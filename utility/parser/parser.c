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
#include "parser_utilities/smaller_parsers/var_parser/var_parser.h"
#include <string.h>
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

static bool handleKeywordInteger_(ParserHandle_t parser, MainFrameHandle_t rootHandle, const Accessibility_t notation);
// static bool tryParseSequence_(const TokenType_t* pattern,const size_t patternSize);
// static bool assignTokenValue_(char** to, const char* from);
// static bool addLibraryForCompilation_(ParserHandle_t parser, ImportObjectHandle_t* importObject);
// static int checkIfPathAlreadyCompiled_(CompilerHandle_t compiler, ImportObjectHandle_t path);
static inline bool handleNotation_(ParserHandle_t parser, MainFrameHandle_t rootHandle);
// static inline bool addDeclaredMethodsToGlobalLinkList_(ParserHandle_t parser, const MainFrameHandle_t root);
// static inline bool addDeclaredMethodsToGlobalLinkListIteratorCallback_(void *key, int count, void *value, void *user);
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
        switch(cTokenType)
        {
            case BIT_TYPE:      handleKeywordInteger_(parser, root, NO_NOTATION);break;  // detected bit keyword
            case NOTATION:      handleNotation_(parser, root);break;                     // detected annotation
            case SEMICOLON:     break;                                                   // detected random semicolon, skip it
            default : Shouter_shoutUnrecognizedToken(cTokenP);break;                     // error case
        }

        currentToken++;

    }

    // Hashmap_forEach(&root->methods, addDeclaredMethodsToGlobalLinkListIteratorCallback_, parser);


    return SUCCESS;
}


// static inline bool addDeclaredMethodsToGlobalLinkListIteratorCallback_(void *key, int count, void *value, void *user)
// {
//     if(!Vector_append(&(((ParserHandle_t)user)->compiler->AllMethodDeclarations), value))
//     {
//         return ERROR;
//     }
// }

static inline bool handleNotation_(ParserHandle_t parser, MainFrameHandle_t rootHandle)
{
    currentToken++;

    if(NAMING)
    {
        // Identifying type by notation naming
        for(uint8_t bindingIdx = 0; bindingIdx < ObjectTypes_getNotationTableSize(); bindingIdx++)
        {
            if(strcmp((*currentToken)->valueString, ObjectTypes_getNotationBindingById(bindingIdx)->naming) == 0)
            {
                currentToken++;
                handleKeywordInteger_(parser, rootHandle, ObjectTypes_getNotationBindingById(bindingIdx)->type);
                return SUCCESS;
            }
        }

        // Situation when passed loop without finding anything
        Shouter_shoutError(cTokenP, "Notation '%s' is not existing in my knowledge", (*currentToken)->valueString);

        currentToken++;
        handleKeywordInteger_(parser, rootHandle, NO_NOTATION);
        
    }else
    {
        Shouter_shoutError(cTokenP, "Notation doesn't have any type declared");
    }
    return SUCCESS;
}


static inline bool handleKeywordInteger_(ParserHandle_t parser, MainFrameHandle_t rootHandle, const Accessibility_t notation)
{
    VariableObjectHandle_t variable;

    ALLOC_CHECK(variable, sizeof(VariableObject_t), ERROR);

    if(!VarParser_parseVariable(&currentToken, variable))
    {
        return ERROR;
    }

    currentToken++;

    if(cTokenType == SEMICOLON)
    {
        variable->assignedValue = 0;

        if(Hashmap_set(&rootHandle->classVariables, variable->objectName, variable))
        {
            Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->objectName);
            return ERROR;
        }

        if(variable->objectName != NULL)
        {
            // If variable fully valid with naming, it counts as space occupying variable, it increases overall object spawn size
            rootHandle->objectSizeBits += variable->bitpack;
        }
        
    }else if(cTokenType == EQUAL)
    {
        Shouter_shoutError(cTokenP, "Variables can be manipulated or assigned only in function scopes");

        if(!ParserUtils_skipUntil(&currentToken, endToken, SEMICOLON))
        {
            return ERROR;
        }
        
        // if(cTokenType == NUMBER_VALUE)   // assignableValue
        // {
        //     NULL_GUARD(cTokenP->valueString, ERROR, Log_e(TAG, "Cannot parse token value cause its NULL"));
            
        //     variable->assignedValue = atoll(cTokenP->valueString);

        //     currentToken++;

        //     if(cTokenType == SEMICOLON)
        //     {

        //         if(Hashmap_set(&rootHandle->classVariables, variable->objectName, variable))
        //         {
        //             Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->objectName);
        //             return ERROR;
        //         }

        //     }else
        //     {
        //         Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
        //         ParserUtils_skipUntil(&currentToken, endToken, SEMICOLON);
        //     }

        // }else
        // {
        //     Shouter_shoutError(cTokenP, "To variable can be assigned constant number or other variable only");
        // }

    }else if(cTokenType == BRACKET_ROUND_START)   // identified method
    {
        currentToken++;
        MethodParser_parseMethod(&currentToken, variable, parser, rootHandle, notation);
    }else
    {
        Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
        ParserUtils_skipUntil(&currentToken, endToken, SEMICOLON);
    }

    return SUCCESS;

}

// static inline bool handleKeywordImport_(ParserHandle_t parser, MainFrameHandle_t rootHandle)
// {
//     ImportObjectHandle_t importObject;

//     ALLOC_CHECK(importObject,sizeof(ImportObject_t), ERROR);

//     currentToken++;
//     if(cTokenType == LITTERAL)                 // detected <
//     {
//         // standart lib detected
//         importObject->name = cTokenP->valueString;

//         if(!addLibraryForCompilation_(parser, &importObject))
//         {
//             Log_e(TAG, "Failed to add library path for paths to compile");
//             return ERROR;
//         }

//         if(Hashmap_set(&rootHandle->imports, importObject->name, importObject))
//         {
//             Shouter_shoutError(cTokenP, "Library \'%s\' is already imported", importObject->name);
//             return ERROR;
//         }

 
//     }else
//     {
//         Shouter_shoutError(cTokenP, "Forgot to define module path...");
//     }
//     // Log_d(TAG, "Current token \'%d\' after libary parse", cTokenP->tokenType);

//     return SUCCESS;
// }

// static inline bool addLibraryForCompilation_(ParserHandle_t parser, ImportObjectHandle_t* importObject)
// {
//     char* newFilePathToCompile;
    
//     size_t libraryRelativePathLength;
//     FILE* fileToCheck;

//     libraryRelativePathLength = strlen((*importObject)->name);

//     // .iguana is longest
//     ALLOC_CHECK(newFilePathToCompile,
//     (parser->currentFolderPathLength + libraryRelativePathLength + LONGEST_POSSIBLE_IGUANA_EXTENSION_LENGTH),
//     ERROR);

//     memcpy(newFilePathToCompile, parser->currentFolderPath, parser->currentFolderPathLength);
//     memcpy(newFilePathToCompile + parser->currentFolderPathLength, (*importObject)->name, libraryRelativePathLength);

//     char* endingExtensionPointer = newFilePathToCompile + parser->currentFolderPathLength + libraryRelativePathLength;
//     *endingExtensionPointer = '.'; // adding extension dot
//     endingExtensionPointer++;

//     for(uint8_t i = 0; i < sizeof(allIguanaExtensions) / sizeof(char*); i++)
//     {

//         uint8_t j;
//         for(j = 0; j < strlen(allIguanaExtensions[i]); j++)
//         {
//             endingExtensionPointer[j] = allIguanaExtensions[i][j]; 
//         }
//         endingExtensionPointer[j] = '\0';

//         fileToCheck = fopen(newFilePathToCompile, "r");
//         if(fileToCheck == NULL)
//         {
//             continue;
//         }else
//         {
//             (*importObject)->realPath = realpath(newFilePathToCompile, NULL);
//             free(newFilePathToCompile);

//             int matchedPosition = checkIfPathAlreadyCompiled_(parser->compiler, *importObject);

//             if(matchedPosition < 0)
//             {
//                 ImportObject_generateRandomIDForObject(*importObject);
//                 Queue_enqueue(&parser->compiler->filePathsToCompile, *importObject);
//             }else
//             {
//                 *importObject = parser->compiler->alreadyCompiledFilePaths.expandable[matchedPosition];
//                 NULL_GUARD(*importObject, ERROR, Log_e(TAG, "Something wrong with memory"));
//             }

//             return SUCCESS;
//         }
//     }
//     Shouter_shoutError(cTokenP, "lib \'%s\'cannot be found", (*importObject)->name);
//     return SUCCESS;
// }


// static inline int checkIfPathAlreadyCompiled_(CompilerHandle_t compiler, ImportObjectHandle_t path)
// {
    
//     for(size_t compiledPathIdx = 0; compiledPathIdx < compiler->alreadyCompiledFilePaths.currentSize; compiledPathIdx++)
//     {
//         // generating absolute paths for better same path checking
//         ImportObjectHandle_t alreadyCompiled = compiler->alreadyCompiledFilePaths.expandable[compiledPathIdx];
//         if(strcmp(alreadyCompiled->realPath, path->realPath) == 0)
//         {
//             return compiledPathIdx;
//         }
//     }
//     return -1;
// }