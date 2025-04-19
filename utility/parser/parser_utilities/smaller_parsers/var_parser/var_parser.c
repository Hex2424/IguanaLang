/**
 * @file method_parser.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2025-04-19
 */

#include "var_parser.h"
#include "../../global_parser_utility.h"

////////////////////////////////
// DEFINES
#define cTokenP (**currentTokenHandle)
#define cTokenType cTokenP -> tokenType
#define cTokenIncrement (*currentTokenHandle)++
////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "VAR_PARSER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool VarParser_parseVariable(TokenHandler_t** currentTokenHandle, VariableObjectHandle_t variableHolder)
{
    if(!ParserUtils_tryParseSequence(currentTokenHandle, PATTERN_VAR_TYPE, PATTERN_VAR_TYPE_SIZE))
    {
        return SUCCESS;
    }

    NULL_GUARD((*(*currentTokenHandle - 1))->valueString, ERROR, Log_e(TAG, "Cannot parse token value cause its NULL"));

    variableHolder->bitpack = atoll((*(*currentTokenHandle - 1))->valueString);
    variableHolder->assignedVariable = NULL;

    if(cTokenType == NAMING)        
    {
        variableHolder->objectName = cTokenP->valueString;
        return SUCCESS;
    }else if(cTokenType == BRACKET_ROUND_START)
    {
        cTokenIncrement;

        if(cTokenType == BRACKET_ROUND_END)
        {
            variableHolder->castedFile = NULL;
        }else if(cTokenType == NONE)
        {
            variableHolder->castedFile = NULL;
            cTokenIncrement;

            if (cTokenType != BRACKET_ROUND_END)
            {
                Shouter_shoutExpectedToken(cTokenP, BRACKET_ROUND_END);
                return SUCCESS;
            }
        }else if(cTokenType == NAMING)
        {
            variableHolder->castedFile = cTokenP->valueString;
            cTokenIncrement;

            if (cTokenType != BRACKET_ROUND_END)
            {
                Shouter_shoutExpectedToken(cTokenP, BRACKET_ROUND_END);
                return SUCCESS;
            }
            
        }else
        {
            Shouter_shoutUnrecognizedToken(cTokenP);
            return SUCCESS;
        }

    }else if(cTokenType == SEMICOLON)
    {
        return SUCCESS;
    }else 
    {
        Shouter_shoutUnrecognizedToken(cTokenP);
        return SUCCESS;
    }

    cTokenIncrement;

    if(cTokenType == NAMING)
    {
        variableHolder->objectName = cTokenP->valueString;
    }else if(cTokenType == SEMICOLON)
    {
        return SUCCESS;
    }else
    {
        Shouter_shoutUnrecognizedToken(cTokenP);
        return SUCCESS;
    }

    return SUCCESS;
}