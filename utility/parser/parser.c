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
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "PARSER";

////////////////////////////////
// PRIVATE TYPES
Vector_t alreadyCompiledFilePaths_;
Vector_t filePathsToCompile_;
TokenHandler_t* tokens;
size_t tokensCount;
size_t currentTokenPos;

////////////////////////////////
// PRIVATE METHODS

static bool isTokenListPosMatchingPattern_(const TokenTypeHandler_t pattern, const size_t patternLength);
static inline bool parseDeclaration_(MainFrameHandle_t rootHandle);
static bool parseMethodParameters_(MethodObjectHandle_t methodObjectHandle);
static bool handleVariableObjectFill_(VariableObjectHandle_t* variableObjectHandle);
static bool parseMethod_(MethodObjectHandle_t methodObject, const VariableObjectHandle_t returnVariable);

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
    currentTokenPos = 0;

    if(!MainFrame_init(&root))
    {
        Log_e(TAG, "MainFrame init failed");
        Vector_destroy(tokenVector);
        return ERROR;
    }
    while (currentTokenPos < tokensCount)
    {
        if(isTokenListPosMatchingPattern_(PATTERN_LIBRARY_IMPORT, PATTERN_LIBRARY_IMPORT_SIZE))
        {
            // detected object import

            if(!Vector_append(root.imports, tokens[currentTokenPos + 2]->valueString))
            {
                Log_e(TAG, "Failed to append library import:%s", tokens[currentTokenPos + 2]->valueString);
                return ERROR;
            }

            currentTokenPos += PATTERN_LIBRARY_IMPORT_SIZE;
        }else
        if(isTokenListPosMatchingPattern_(PATTERN_DECLARE, PATTERN_DECLARE_SIZE))
        {
            // detected declared object variable
            if(!parseDeclaration_(&root))
            {
                Log_e(TAG, "Failed to parse variable declaration");
                return ERROR;
            }

        }else
        {
            // undetected any token type

            Log_e(TAG, "Unrecognised token:%d", tokens[currentTokenPos]->tokenType);
            return ERROR;
        }
    }

    

    return SUCCESS;
}


static bool handleVariableObjectFill_(VariableObjectHandle_t* variableObjectHandle)
{
    char* content;

    content = tokens[currentTokenPos + 2]->valueString;
    NULL_GUARD(content, ERROR, Log_e(TAG, "Content is null at token parsing"));

    (*variableObjectHandle) = malloc(sizeof(VariableObject_t));
    ALLOC_CHECK((*variableObjectHandle), ERROR);

    (*variableObjectHandle)->bitpack = atoi(content);
    (*variableObjectHandle)->variableName = tokens[currentTokenPos + 3]->valueString;

    return SUCCESS;
}

/**
 * @brief Private method for parsing variable type declarations
 * 
 * @param[out] rootHandle   handle to root class
 * @return                  Success State
 */
static inline bool parseDeclaration_(MainFrameHandle_t rootHandle)
{
    VariableObjectHandle_t variableDeclaration;

    if(!handleVariableObjectFill_(&variableDeclaration))
    {
        Log_e(TAG, "Failed to handle VariableObjectFilling");
        return ERROR;
    }

    currentTokenPos += PATTERN_DECLARE_SIZE;

    if(isTokenListPosMatchingPattern_(PATTERN_SEMICOLON, PATTERN_SEMICOLON_SIZE))
    {
        // detected variable declaration

        if(!Vector_append(rootHandle->classVariables, variableDeclaration))
        {
            Log_e(TAG, "Failed to append variableDeclaration: %s", variableDeclaration->variableName);
            return ERROR;
        }
        currentTokenPos += PATTERN_SEMICOLON_SIZE;
        
    }else
    if(isTokenListPosMatchingPattern_(PATTERN_BRACKET_ROUND_START, PATTERN_BRACKET_ROUND_START_SIZE))
    {
        // detected method declaration
        MethodObjectHandle_t methodObject;
        
        methodObject = malloc(sizeof(MethodObject_t));
        ALLOC_CHECK(methodObject, ERROR);

        if(!parseMethod_(methodObject, variableDeclaration))
        {
            Log_e(TAG, "Failed to parse method");
            return ERROR;
        }

    }else
    {
        Log_e(TAG, "Unexpected declaration of %s", tokens[currentTokenPos]->valueString);
        return ERROR;
    }

    return SUCCESS;
}


static bool parseMethod_(MethodObjectHandle_t methodObject, const VariableObjectHandle_t returnVariable)
{

    methodObject->returnVariable = returnVariable;
    methodObject->parameters = malloc(sizeof(Vector_t));
    
    ALLOC_CHECK(methodObject->parameters, ERROR);

    if(!Vector_create(methodObject->parameters, NULL))
    {
        Log_e(TAG, "Failed to create vector for parameters of method");
        return ERROR;
    }

    currentTokenPos += PATTERN_BRACKET_ROUND_START_SIZE;

    if(!parseMethodParameters_(methodObject))
    {
        Log_e(TAG, "Failed to parse method parameters");
        return ERROR;
    }

    return SUCCESS;
}

/**
 * @brief Private method for parsing method parameters variables
 * 
 * @param[out] methodObjectHandle   handle to method object 
 * @return                          Success state
 */
static bool parseMethodParameters_(MethodObjectHandle_t methodObjectHandle)
{
    while (true)
    {
        if(isTokenListPosMatchingPattern_(PATTERN_DECLARE, PATTERN_DECLARE_SIZE))
        {
            VariableObjectHandle_t parameter;

            if(!handleVariableObjectFill_(&parameter))
            {
                Log_e(TAG, "Failed to handle parametersFill");
                return ERROR;
            }

            if(!Vector_append(methodObjectHandle->parameters, parameter))
            {
                Log_e(TAG, "Failed to handle parametersFill");
                return ERROR;
            }
            
            currentTokenPos += PATTERN_DECLARE_SIZE;
            
            if(isTokenListPosMatchingPattern_(PATTERN_COMMA, PATTERN_COMMA_SIZE))
            {
                currentTokenPos += PATTERN_COMMA_SIZE;
                continue; // continuing on next parameter
            }else
            if(isTokenListPosMatchingPattern_(PATTERN_BRACKET_ROUND_END, PATTERN_BRACKET_ROUND_END_SIZE))
            {
                currentTokenPos += PATTERN_BRACKET_ROUND_END_SIZE;
                break;
            }else
            {
                Log_e(TAG, "Expected \")\" at the end of method parameters declaration, not a %d", tokens[currentTokenPos]);
                return ERROR;
            }


        }else
        {
            Log_e(TAG, "Unexpected token");
            return ERROR;
        }
    }
    return SUCCESS;
    
}

/**
 * @brief Private method for recognising pattern of specific expression
 * 
 * @param[in] pattern           pattern which to try recognize
 * @param[in] patternLength     length of pattern
 * @return                      Success State 
 */
static bool isTokenListPosMatchingPattern_(const TokenTypeHandler_t pattern, const size_t patternLength)
{   
    size_t patternIdx;
    size_t patternTokenPos;

    patternTokenPos = currentTokenPos;

    if (patternTokenPos + (patternLength - 1) > tokensCount)
    {
        Log_e(TAG, "Code contains junk at the end");
        return ERROR;
    }
    patternIdx = 0;

    while(patternIdx < patternLength)
    {
        if (tokens[patternTokenPos]->tokenType != pattern[patternIdx])
        {
            return false;
        }
        patternIdx += 1;
        patternTokenPos += 1;

    }

    return true;
}
