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
    variableHolder->castedFile = NULL;
    variableHolder->objectName = NULL;
    variableHolder->bitpack = 0;

    if(!ParserUtils_tryParseSequence(currentTokenHandle, PATTERN_VAR_TYPE, PATTERN_VAR_TYPE_SIZE))
    {
        return SUCCESS;
    }

    NULL_GUARD((*(*currentTokenHandle - 1))->valueString, ERROR, Log_e(TAG, "Cannot parse token value cause its NULL"));

    variableHolder->bitpack = atoll((*(*currentTokenHandle - 1))->valueString);

    if(cTokenType == NAMING)        
    {
        variableHolder->objectName = cTokenP->valueString;
        return SUCCESS;
    }else if(cTokenType == ARROW_LEFT)
    {
        cTokenIncrement;

        if(cTokenType == ARROW_RIGHT)
        {
            variableHolder->castedFile = NULL;
        }else if(cTokenType == NONE)
        {
            variableHolder->castedFile = NULL;
            cTokenIncrement;

            if (cTokenType != ARROW_RIGHT)
            {
                Shouter_shoutExpectedToken(cTokenP, ARROW_RIGHT);
                return SUCCESS;
            }
        }else if(cTokenType == NAMING)
        {
            variableHolder->castedFile = cTokenP->valueString;
            cTokenIncrement;

            if (cTokenType != ARROW_RIGHT)
            {
                Shouter_shoutExpectedToken(cTokenP, ARROW_RIGHT);
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


VariableObjectHandle_t VarParser_searchVariableInVectorByName(const VectorHandler_t vectorHandle, const char* name)
{
    for(uint32_t variableIndex = 0; variableIndex < vectorHandle->currentSize; variableIndex++)
    {
        const VariableObjectHandle_t variableCurrent = vectorHandle->expandable[variableIndex];
        
        if(strcmp(variableCurrent->objectName, name) == 0)
        {
            return variableCurrent;
        }
    }

    return NULL;
}


void VarParser_printVarsInVector(const VectorHandler_t vectorHandle, const char* name)
{
    Log_i(TAG, "Variables Vector: %s", name);
    Log_i(TAG, "==============================");
    
    for(uint32_t vectorIndex = 0; vectorIndex < vectorHandle->currentSize; vectorIndex++)
    {
        VariableObjectHandle_t varHandle = (VariableObjectHandle_t) vectorHandle->expandable[vectorIndex];

        if (varHandle == NULL)
        {
            Log_i(TAG, "NULL variable at index:%u", vectorIndex);
        }else
        {
            Log_i(TAG, "bit:%lu, pos:%u group:%lu", varHandle->bitpack, varHandle->posBit, varHandle->belongToGroup);
        }
    }
    Log_i(TAG, "==============================");
}