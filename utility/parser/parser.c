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
#include "parser_utilities/smaller_parsers/method_parsers.h"
#include "string.h"

////////////////////////////////
// DEFINES

#define cTokenP (*currentToken)
#define cTokenType cTokenP -> tokenType
#define LONGEST_POSSIBLE_IGUANA_EXTENSION_LENGTH sizeof("iguana")
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
static bool handleKeywordInteger_(ParserHandle_t parser, MainFrameHandle_t rootHandle);
static bool tryParseSequence_(const TokenType_t* pattern,const size_t patternSize);
static bool assignTokenValue_(char** to, const char* from);
static bool addLibraryForCompilation_(ParserHandle_t parser, const char* libraryRelativePath);

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
        if(cTokenType == INTEGER_TYPE)      // detected int keyword
        {
            handleKeywordInteger_(parser, root);
        }else
        {
            Shouter_shoutUnrecognizedToken(cTokenP);
        }
        currentToken++;

    }

    Log_i(TAG, "Compiling completed with %d errors", Shouter_getErrorCount());
    return SUCCESS;
}

static inline bool handleKeywordInteger_(ParserHandle_t parser, MainFrameHandle_t rootHandle)
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

        if(!Vector_append(rootHandle->classVariables, variable))
        {
            Log_e(TAG, "Failed to append new class variable");
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
                if(!Vector_append(rootHandle->classVariables, variable))
                {
                    Log_e(TAG, "Failed to append new class variable");
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
        if(!addLibraryForCompilation_(parser, importObject->name))
        {
            Log_e(TAG, "Failed to add library path for paths to compile");
            return ERROR;
        }
        currentToken++;


        if(cTokenType != SEMICOLON)
        {
            Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
        }else
        {
            if(!Vector_append(rootHandle->imports, importObject))
            {
                Log_e(TAG, "Couldn't append libary to vector");
                return ERROR;
            }

        }
 
    }else
    {
        Shouter_shoutError(cTokenP, "Forgot to define module path...");
    }
    // Log_d(TAG, "Current token \'%d\' after libary parse", cTokenP->tokenType);

    return SUCCESS;
}

static inline bool addLibraryForCompilation_(ParserHandle_t parser, const char* libraryRelativePath)
{
    char* newFilePathToCompile;
    
    size_t libraryRelativePathLength;
    FILE* fileToCheck;

    libraryRelativePathLength = strlen(libraryRelativePath);

    newFilePathToCompile = malloc(parser->currentFolderPathLength + libraryRelativePathLength + LONGEST_POSSIBLE_IGUANA_EXTENSION_LENGTH); // .iguana is longest
    ALLOC_CHECK(newFilePathToCompile, ERROR)

    memcpy(newFilePathToCompile, parser->currentFolderPath, parser->currentFolderPathLength);
    memcpy(newFilePathToCompile + parser->currentFolderPathLength, libraryRelativePath, libraryRelativePathLength);

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
            Queue_enqueue(&parser->compiler->filePathsToCompile, newFilePathToCompile);
            return SUCCESS;
        }
    }
    Shouter_shoutError(cTokenP, "lib cannot be found");
    return SUCCESS;
}