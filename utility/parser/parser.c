/**
 * @file parser.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-07
 */

#include "parser.h"
#include "../tokenizer/token/token.h"
#include "parser_utilities/global_parser_utility.h"
#include "parser_utilities/smaller_parsers/method_parser/method_parsers.h"
#include "string.h"
#include "structures/import_object/import_object.h"
#include "../hash/random/random.h"
////////////////////////////////
// DEFINES

#define cTokenP                                     (*currentToken)
#define cTokenType                                  cTokenP -> tokenType
#define LONGEST_POSSIBLE_IGUANA_EXTENSION_LENGTH    sizeof("iguana")


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "PARSER";


// Ordering affects speed, the most common should be first element
const char* allIguanaExtensions[] = 
{
    "i",
    "ig",
    "iguana"
};

////////////////////////////////
// PRIVATE TYPES

TokenHandler_t* tokens;
TokenHandler_t* currentToken;
TokenHandler_t* endToken;
size_t tokensCount;
////////////////////////////////
// PRIVATE METHODS

static bool handleKeywordImport_(ParserHandle_t parser, MainFrameHandle_t rootHandle);
static bool handleKeywordInteger_(MainFrameHandle_t rootHandle);
static bool tryParseSequence_(const TokenType_t* pattern,const size_t patternSize);
static bool assignTokenValue_(char** to, const char* from);
static bool addLibraryForCompilation_(ParserHandle_t parser, ImportObjectHandle_t* importObject);
static int checkIfPathAlreadyCompiled_(CompilerHandle_t compiler, ImportObjectHandle_t path);

////////////////////////////////
// IMPLEMENTATION

/**
 * @brief   Public method for initializing Parser
 * 
 * @return  Success State
 */
bool Parser_initialize(ParserHandle_t parser)
{
    return SUCCESS;
}

bool Parser_destroy(ParserHandle_t parser)
{
    return SUCCESS;
}

/**
 * @brief Public method used for parsing tokens by provided token list
 * 
 * @param[out] root         Igauana code AST goes here as main root
 * @param[in] tokenList     file tokens go here 
 * @return                  Success state
 */ 
bool Parser_parseTokens(ParserHandle_t parser, MainFrameHandle_t root, const VectorHandler_t tokenVector)
{
    tokens = (TokenHandler_t*) tokenVector->expandable;
    tokensCount = tokenVector->currentSize;
    currentToken = tokens;
    endToken = tokens + tokenVector->currentSize;

    if(!MainFrame_init(root))
    {
        Log_e(TAG, "MainFrame init failed");
        Vector_destroy(tokenVector);
        return ERROR;
    }
    while (currentToken < endToken)
    {
        if(cTokenType == MODULE_IMPORT)     // detected import
        {
            handleKeywordImport_(parser, root);
        }else
        if(cTokenType == BIT_TYPE)      // detected int keyword
        {
            handleKeywordInteger_(root);
        }else
        {
            Shouter_shoutUnrecognizedToken(cTokenP);
        }
        currentToken++;

    }
    return SUCCESS;
}

static inline bool handleKeywordInteger_(MainFrameHandle_t rootHandle)
{
    VariableObjectHandle_t variable;

    if(!ParserUtils_tryParseSequence(&currentToken, PATTERN_DECLARE, PATTERN_DECLARE_SIZE))
    {
        return SUCCESS;
    }
    variable = malloc(sizeof(VariableObject_t));
    ALLOC_CHECK(variable, ERROR);

    variable->variableName = (*(currentToken - 1))->valueString;
    variable->bitpack = atoi((*(currentToken - 2))->valueString);
    variable->assignedVariable = NULL;


    if(cTokenType == SEMICOLON)        
    {
        variable->assignedValue = 0;

        if(!Hashmap_putEntry(&rootHandle->classVariables, variable->variableName, variable))
        {
            Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->variableName);
            return ERROR;
        }
        
    }else
    if(cTokenType == EQUAL)             // checking for assignable declaration
    {
        currentToken++;
        if(cTokenType = NUMBER_VALUE)   // assignableValue
        {
            NULL_GUARD(cTokenP->valueString, ERROR, Log_e(TAG, "Cannot parse token value cause its NULL"));

            variable->assignedValue = atoll(cTokenP->valueString);
            currentToken++;

            if(cTokenType == SEMICOLON)
            {

                if(!Hashmap_putEntry(&rootHandle->classVariables, variable->variableName, variable))
                {
                    Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->variableName);
                    return ERROR;
                }

            }else
            {
                Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
            }

        }else
        {
            Shouter_shoutError(cTokenP, "To variable can be assigned constant number or other variable only");
        }

    }else
    if(cTokenType == BRACKET_ROUND_START)   // identified method
    {
        currentToken++;
        MethodParser_parseMethod(&currentToken, rootHandle);

    }else
    {
        Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
    }

    return SUCCESS;

}


static inline bool handleKeywordImport_(ParserHandle_t parser, MainFrameHandle_t rootHandle)
{
    ImportObjectHandle_t importObject;

    importObject = malloc(sizeof(ImportObject_t));
    ALLOC_CHECK(importObject, ERROR);

    currentToken++;
    if(cTokenType == LITTERAL)                 // detected <
    {
        // standart lib detected
        importObject->name = cTokenP->valueString;

        if(!addLibraryForCompilation_(parser, &importObject))
        {
            Log_e(TAG, "Failed to add library path for paths to compile");
            return ERROR;
        }

        if(!Hashmap_putEntry(&rootHandle->imports, importObject->name, importObject))
        {
            Shouter_shoutError(cTokenP, "Library \'%s\' is already imported", importObject->name);
            return ERROR;
        }

 
    }else
    {
        Shouter_shoutError(cTokenP, "Forgot to define module path...");
    }
    // Log_d(TAG, "Current token \'%d\' after libary parse", cTokenP->tokenType);

    return SUCCESS;
}

static inline bool addLibraryForCompilation_(ParserHandle_t parser, ImportObjectHandle_t* importObject)
{
    char* newFilePathToCompile;
    
    size_t libraryRelativePathLength;
    FILE* fileToCheck;

    libraryRelativePathLength = strlen((*importObject)->name);

    newFilePathToCompile = malloc(parser->currentFolderPathLength + libraryRelativePathLength + LONGEST_POSSIBLE_IGUANA_EXTENSION_LENGTH); // .iguana is longest
    ALLOC_CHECK(newFilePathToCompile, ERROR)

    memcpy(newFilePathToCompile, parser->currentFolderPath, parser->currentFolderPathLength);
    memcpy(newFilePathToCompile + parser->currentFolderPathLength, (*importObject)->name, libraryRelativePathLength);

    char* endingExtensionPointer = newFilePathToCompile + parser->currentFolderPathLength + libraryRelativePathLength;
    *endingExtensionPointer = '.'; // adding extension dot
    endingExtensionPointer++;

    for(uint8_t i = 0; i < sizeof(allIguanaExtensions) / sizeof(char*); i++)
    {

        uint8_t j;
        for(j = 0; j < strlen(allIguanaExtensions[i]); j++)
        {
            endingExtensionPointer[j] = allIguanaExtensions[i][j]; 
        }
        endingExtensionPointer[j] = '\0';

        fileToCheck = fopen(newFilePathToCompile, "r");
        if(fileToCheck == NULL)
        {
            continue;
        }else
        {
            (*importObject)->realPath = realpath(newFilePathToCompile, NULL);
            
            int matchedPosition = checkIfPathAlreadyCompiled_(parser->compiler, *importObject);

            if(matchedPosition < 0)
            {
                ImportObject_generateRandomIDForObject(*importObject);
                Queue_enqueue(&parser->compiler->filePathsToCompile, *importObject);
            }else
            {
                *importObject = parser->compiler->alreadyCompiledFilePaths.expandable[matchedPosition];
                NULL_GUARD(*importObject, ERROR, Log_e(TAG, "Something wrong with memory"));
            }

            return SUCCESS;
        }
    }
    Shouter_shoutError(cTokenP, "lib \'%s\'cannot be found", (*importObject)->name);
    return SUCCESS;
}


static inline int checkIfPathAlreadyCompiled_(CompilerHandle_t compiler, ImportObjectHandle_t path)
{

    for(size_t compiledPathIdx = 0; compiledPathIdx < compiler->alreadyCompiledFilePaths.currentSize; compiledPathIdx++)
    {
        // generating absolute paths for better same path checking
        ImportObjectHandle_t alreadyCompiled = compiler->alreadyCompiledFilePaths.expandable[compiledPathIdx];
        if(strcmp(alreadyCompiled->realPath, path->realPath) == 0)
        {
            return compiledPathIdx;
        }
    }
    return -1;
}