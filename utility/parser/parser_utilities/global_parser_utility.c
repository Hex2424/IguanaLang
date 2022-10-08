/**
 * @file global_parser_utility.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-09
 */

#include "global_parser_utility.h"
#include "../../misc/safety_macros.h"
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "GLOBAL_PARSER_UTILITY";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool ParserUtils_tryParseSequence(TokenHandler_t** currentToken, const TokenType_t* pattern,const size_t patternSize)
{
    int patternIdx;
    for(patternIdx = 0; patternIdx < patternSize; patternIdx++, (*currentToken)++)
    {

        if((**currentToken)->tokenType != pattern[patternIdx])
        {
            // Log_d(TAG, "%d %d", cTokenType,  pattern[patternIdx]);
            if(((patternIdx + 1) < patternSize) && ((**currentToken)->tokenType  == pattern[patternIdx + 1]))
            {
                Shouter_shoutForgottenToken((**currentToken),pattern[patternIdx + 1]);
                
            }else
            {
                Shouter_shoutExpectedToken((**currentToken), pattern[patternIdx]);
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
bool ParserUtils_assignTokenValue(char** to, const char* from)
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

