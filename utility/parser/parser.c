/**
 * @file parser.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
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
TokenHandler_t tokens;
size_t tokensCount;
size_t currentTokenPos;

////////////////////////////////
// PRIVATE METHODS


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


/**
 * @brief Public method used for parsing tokens by provided token list
 * 
 * @param[in] tokenList - file tokens go here 
 * @return Success state
 */
bool Parser_parseTokens(const VectorHandler_t tokenVector)
{
    MainFrame_t root;

    tokens = tokenVector->expandable;
    tokensCount = tokenVector->currentSize;
    currentTokenPos = 0;

    if(!MainFrame_init(&root))
    {
        Log_e(TAG, "MainFrame init failed");
        Vector_destroy(tokenVector);
        return ERROR;
    }

    if(isTokenListPosMatchingPattern_(PATTERN_LIBRARY_IMPORT, sizeof(PATTERN_LIBRARY_IMPORT)))
    {

        


    }else 
    if(isTokenListPosMatchingPattern_())
    {

    }
    
    // imports
    // variables
    // methods    
    

    return SUCCESS;
}


bool isTokenListPosMatchingPattern_(const TokenTypeHandler_t pattern, const size_t patternLength)
{   
    size_t patternIdx;

    if (currentTokenPos + (patternLength - 1) > tokensCount)
    {
        Log_e(TAG, "Code contains junk at the end");
        return ERROR;
    }
    patternIdx = 0;

    while(patternIdx < patternLength)
    {
        if (tokens[currentTokenPos].tokenType != pattern[patternIdx])
        {
            return false;
        }
        patternIdx += 1;
        currentTokenPos += 1;

    }

    return true;
}



