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
#include <string.h>
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
// static const char* TAG = "GLOBAL_PARSER_UTILITY";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool ParserUtils_tryParseSequence(TokenHandler_t** currentTokenHandle, const TokenType_t* pattern,const size_t patternSize)
{
    bool parseCorrect = true;
    for(uint8_t patternIdx = 0; patternIdx < patternSize; patternIdx++, (*currentTokenHandle)++)
    {
        if((**currentTokenHandle)->tokenType == END_FILE)
        {
            break;
        }

        if((**currentTokenHandle)->tokenType != pattern[patternIdx])
        {
            // Log_d(TAG, "%d %d", cTokenType,  pattern[patternIdx]);

            Shouter_shoutExpectedToken((**currentTokenHandle), pattern[patternIdx]);
            parseCorrect = false;
        }
        
    }

    return parseCorrect;
}


bool ParserUtils_skipUntil(TokenHandler_t** currentTokenHandle, const TokenType_t* untilTokenTypeList, const uint8_t tokensLength)
{
    while((**currentTokenHandle)->tokenType != END_FILE)
    {
        for(uint8_t testIdx = 0; testIdx < tokensLength; testIdx++)
        {
            if((**currentTokenHandle)->tokenType == untilTokenTypeList[testIdx])
            {
                return true;
            }
        }

        (*currentTokenHandle)++;
    }

    return false;
}