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

bool isTokenListPosMatchingPattern_(const TokenTypeHandler_t pattern, const size_t patternLength);

////////////////////////////////
// IMPLEMENTATION

/**
 * @brief Public method for initializing Parser
 * 
 * @return Success State
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
 * @param[in] tokenList - file tokens go here 
 * @return Success state
 */ // imports
    // variables
    // methods    

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

            char* content;
            content = tokens[currentTokenPos + 2]->valueString;
            printf("%d\n", currentTokenPos);

            if(content == NULL)
            {
                Log_e(TAG, "Content is null at token parsing");
                return ERROR;
            }

            int bitpack = atoi(content); 
            char* name =  tokens[currentTokenPos + 3]->valueString;

            currentTokenPos += PATTERN_DECLARE_SIZE;


            if(isTokenListPosMatchingPattern_(PATTERN_SEMICOLON, PATTERN_SEMICOLON_SIZE))
            {
                // detected variable declaration
                
                
            }else
            if(isTokenListPosMatchingPattern_(PATTERN_BRACKET_ROUND_START, PATTERN_BRACKET_ROUND_START_SIZE))
            {

            }else
            {
                
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




bool isTokenListPosMatchingPattern_(const TokenTypeHandler_t pattern, const size_t patternLength)
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
