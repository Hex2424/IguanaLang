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
#include "structures/pattern/pattern.h"
#include "structures/method/method.h"
#include "parser_utilities/compiler_messages.h"
#include "string.h"
////////////////////////////////
// DEFINES

#define cTokenP (*currentToken)
#define cTokenType cTokenP -> tokenType

////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "PARSER";

////////////////////////////////
// PRIVATE TYPES
Vector_t alreadyCompiledFilePaths_;
Vector_t filePathsToCompile_;
TokenHandler_t* tokens;
TokenHandler_t* currentToken;
TokenHandler_t* endToken;
size_t tokensCount;
////////////////////////////////
// PRIVATE METHODS

static inline bool handleKeywordImport_(MainFrameHandle_t rootHandle);
static inline bool handleKeywordInteger_(MainFrameHandle_t rootHandle);
static bool tryParseSequence_(const TokenType_t* pattern,const size_t patternSize);
static bool assignTokenValue_(char** to, const char* from);

////////////////////////////////
// IMPLEMENTATION

/**
 * @brief   Public method for initializing Parser
 * 
 * @return  Success State
 */
bool Parser_initialize()
{
    if(!Vector_create(&alreadyCompiledFilePaths_, NULL))
    {
        Log_e(TAG, "Failed to create alreadyCompiledPaths_ vector");
        return ERROR;
    }

    if(!Vector_create(&filePathsToCompile_, NULL))
    {
        Log_e(TAG, "Failed to create filePathsToCompile_ vector");
        return ERROR;
    }
    return SUCCESS;
}

bool Parser_destroy()
{
    if(!Vector_destroy(&alreadyCompiledFilePaths_))
    {
        Log_e(TAG, "Failed to destroy parser alreadyCompiledFilePaths vector");
        return ERROR;
    }

    if(!Vector_destroy(&filePathsToCompile_))
    {
        Log_e(TAG, "Failed to destroy parser filePathsToCompile vector");
        return ERROR;
    }
    return SUCCESS;
}

/**
 * @brief Public method used for parsing tokens by provided token list
 * 
 * @param[in] tokenList     file tokens go here 
 * @return                  Success state
 */ 
bool Parser_parseTokens(const VectorHandler_t tokenVector)
{
    MainFrame_t root;

    tokens = (TokenHandler_t*) tokenVector->expandable;
    tokensCount = tokenVector->currentSize;
    currentToken = tokens;
    endToken = tokens + tokenVector->currentSize;

    if(!MainFrame_init(&root))
    {
        Log_e(TAG, "MainFrame init failed");
        Vector_destroy(tokenVector);
        return ERROR;
    }
    while (currentToken < endToken)
    {
        if(cTokenType == MODULE_IMPORT)     // detected import
        {
            handleKeywordImport_(&root);
        }else
        if(cTokenType == INTEGER_TYPE)      // detected int keyword
        {
            handleKeywordInteger_(&root);
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


    if(!tryParseSequence_(PATTERN_DECLARE, PATTERN_DECLARE_SIZE))
    {
        return SUCCESS;
    }
    variable = malloc(sizeof(VariableObject_t));
    ALLOC_CHECK(variable, ERROR);

    variable->variableName = (*currentToken)->valueString;
    variable->bitpack = atoi((*(currentToken - 1))->valueString);
    variable->assignedVariable = NULL;

    currentToken++;

    if(cTokenType == SEMICOLON)        
    {
        variable->assignedValue = 0;
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
        

    }else
    {
        Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
    }

    return SUCCESS;

}

static inline bool parseMethod_(MainFrame_t root)
{
    // MethodObject_t method;
    
    // if(!parseMethodParameters_(&method))
    // {

    // }
}


// static bool parseMethodParameters_(MethodObjectHandle_t methodHandle)
// {

// }


static bool tryParseSequence_(const TokenType_t* pattern,const size_t patternSize)
{
    int patternIdx;
    for(patternIdx = 0; patternIdx < patternSize; patternIdx++, ++currentToken)
    {
        if(cTokenType != pattern[patternIdx])
        {
            if(((patternIdx + 1) < patternSize) && (cTokenType == pattern[patternIdx + 1]))
            {
                Shouter_shoutForgottenToken(cTokenP, pattern[patternIdx + 1]);
                
            }else
            {
                Shouter_shoutExpectedToken(cTokenP, pattern[patternIdx]);
            }
            return false;
        }
        
    }
    return true;


}


/**
 * @brief Private method for copying and doing malloc manually from token value
 * 
 * @param[out] to   Pointer to Pointer which all bytes will be coppied also being allocated dynamically
 * @param[in] from  Pointer from which all bytes will be coppied
 * @return          Success state
 */
static bool assignTokenValue_(char** to, const char* from)
{
    *to = malloc(strlen(from));     // recopying token value
    ALLOC_CHECK(to, ERROR);
    if(strcpy(*to, from) == NULL)
    {
        Log_e(TAG, "For some reason couldn't copy from token value");
        return ERROR;
    }

    return SUCCESS;
}


static inline bool handleKeywordImport_(MainFrameHandle_t rootHandle)
{
    ImportObjectHandle_t importObject;

    importObject = malloc(sizeof(ImportObject_t));
    ALLOC_CHECK(importObject, ERROR);

    currentToken++;
    if(cTokenType == ARROW_LEFT || cTokenType == LITTERAL)                 // detected <
    {
        // standart lib detected
        currentToken++;

        if(cTokenType == NAMING)
        {
            if(!assignTokenValue_(&importObject->name, cTokenP->valueString))
            {
                Log_e(TAG, "Couldn't assign value from token");
                return ERROR;
            }

            currentToken++;
            
            if(cTokenType == ARROW_LEFT || cTokenType == LITTERAL)
            {
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
                Shouter_shoutExpectedToken(cTokenP, LITTERAL);
            }

        }else
        {
            Shouter_shoutError(cTokenP, "Forgot to define module path...");
        }
        
    }else
    {
        Shouter_shoutExpectedToken(cTokenP, LITTERAL);
    }

    Log_d(TAG, "Current token \'%d\' after libary parse", cTokenP->tokenType);

    return SUCCESS;
}