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
 * @date 2022-10-09
 */

#include "method_parsers.h"
#include "../global_parser_utility.h"
////////////////////////////////
// DEFINES
#define cTokenP (**currentTokenHandle)
#define cTokenType cTokenP -> tokenType

////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "METHOD_PARSER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS

static bool parseMethodParameters_(TokenHandler_t** currentTokenHandle, MethodObjectHandle_t methodHandle);
static bool parseMethodBody_(TokenHandler_t** currentTokenHandle, MethodObjectHandle_t methodHandle);
////////////////////////////////
// IMPLEMENTATION

inline bool MethodParser_parseMethod(TokenHandler_t** currentTokenHandle, MainFrameHandle_t root)
{
    MethodObjectHandle_t methodHandle;

    methodHandle = malloc(sizeof(MethodObject_t));
    ALLOC_CHECK(methodHandle, ERROR);

    methodHandle->methodName = (*(*currentTokenHandle - 2))->valueString;
    

    if(!parseMethodParameters_(currentTokenHandle, methodHandle))
    {
        return ERROR;                         // getting back because parameters parsing failed
    }

    if(!parseMethodBody_(currentTokenHandle, methodHandle))
    {
        return ERROR;
    }

    if(!Vector_append(root->methods, methodHandle))
    {
        Log_e(TAG, "Failed to append to method to methods vector");
        return ERROR;
    }

}

static bool parseMethodParameters_(TokenHandler_t** currentTokenHandle, MethodObjectHandle_t methodHandle)
{
    methodHandle->parameters = malloc(sizeof(Vector_t));
    ALLOC_CHECK(methodHandle->parameters, ERROR);

    if(!Vector_create(methodHandle->parameters, NULL))
    {
        Log_e(TAG, "Failed to create vector for method paramters for some reason");
        return ERROR;
    }

    while(true)
    {
        VariableObjectHandle_t parameter;

        parameter = malloc(sizeof(VariableObject_t));
        ALLOC_CHECK(parameter, ERROR);

        if(cTokenType == INTEGER_TYPE)
        {
            if(!ParserUtils_tryParseSequence(currentTokenHandle, PATTERN_DECLARE, PATTERN_DECLARE_SIZE))
            {
                (*currentTokenHandle)++;
                continue;
            }
            
            parameter->variableName = (*((*currentTokenHandle) - 1))->valueString;
            parameter->bitpack = atoll((*((*currentTokenHandle) - 2))->valueString);

            if(!Vector_append(methodHandle->parameters, parameter))
            {
                Log_e(TAG, "Failed to append to parameters vector");
                return ERROR;
            }

            if(cTokenType == COMMA)
            {
                (*currentTokenHandle)++;
                continue;
            }else 
            if(cTokenType == BRACKET_ROUND_END)
            {
                break;
            }else
            {
                Shouter_shoutExpectedToken(cTokenP, BRACKET_ROUND_END);
                (*currentTokenHandle)++;
                continue;
            }

        }else
        if(cTokenType == BRACKET_ROUND_END)
        {
            break;
        }else
        {
            Shouter_shoutExpectedToken(cTokenP, INTEGER_TYPE);
            (*currentTokenHandle)++;
            continue;
        }
        
    }
    (*currentTokenHandle)++;
    return SUCCESS;

}

static bool parseMethodBody_(TokenHandler_t** currentTokenHandle, MethodObjectHandle_t methodHandle)
{
    if(cTokenType == BRACKET_START)
    {
        (*currentTokenHandle)++;
        if(cTokenType == BRACKET_END)
        {
            return SUCCESS;
        }
    }else
    {
        Shouter_shoutExpectedToken(cTokenP, BRACKET_START);
    }
    
    return ERROR;
}