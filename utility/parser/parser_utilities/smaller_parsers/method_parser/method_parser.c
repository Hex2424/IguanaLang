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
#include "../../global_parser_utility.h"
#include "../body_parser/body_parser.h"

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
static bool parseMethodReturnVariable_(TokenHandler_t** currentTokenHandle, MethodObjectHandle_t methodHandle);

////////////////////////////////
// IMPLEMENTATION

inline bool MethodParser_parseMethod(TokenHandler_t** currentTokenHandle, MainFrameHandle_t root)
{
    MethodObjectHandle_t methodHandle;

    ALLOC_CHECK(methodHandle, sizeof(MethodObject_t), ERROR);
    // setting up method name

    if(!parseMethodReturnVariable_(currentTokenHandle, methodHandle))
    {
        return ERROR;
    }
    if(!parseMethodParameters_(currentTokenHandle, methodHandle))
    {
        return ERROR;                         // getting back because parameters parsing failed
    }

    if(!parseMethodBody_(currentTokenHandle, methodHandle))
    {
        return ERROR;
    }

    if(!Hashmap_putEntry(&root->methods, methodHandle->methodName, methodHandle))
    {
        Shouter_shoutError(cTokenP, "Method \'%s\' is declared several times", methodHandle->methodName);
        return ERROR;
    }


}

static bool parseMethodReturnVariable_(TokenHandler_t** currentTokenHandle, MethodObjectHandle_t methodHandle)
{

    methodHandle->methodName = (*(*currentTokenHandle - 2))->valueString;
    
    methodHandle->returnVariable.variableName = methodHandle->methodName;
    methodHandle->returnVariable.bitpack = atoi((*((*currentTokenHandle) - 3))->valueString);
    methodHandle->returnVariable.assignedValue = 0;
    methodHandle->returnVariable.assignedVariable = NULL;
    
    return SUCCESS;
}


static bool parseMethodParameters_(TokenHandler_t** currentTokenHandle, MethodObjectHandle_t methodHandle)
{

    ALLOC_CHECK(methodHandle->parameters, sizeof(Vector_t), ERROR);

    if(!Vector_create(methodHandle->parameters, NULL))
    {
        Log_e(TAG, "Failed to create vector for method paramters for some reason");
        return ERROR;
    }

    while(true)
    {
        VariableObjectHandle_t parameter;

        ALLOC_CHECK(parameter, sizeof(VariableObject_t), ERROR);

        if(cTokenType == BIT_TYPE)
        {
            if(!ParserUtils_tryParseSequence(currentTokenHandle, PATTERN_DECLARE, PATTERN_DECLARE_SIZE))
            {
                (*currentTokenHandle)++;
                continue;
            }
            
            parameter->variableName = (*((*currentTokenHandle) - 1))->valueString;
            parameter->bitpack = atoi((*((*currentTokenHandle) - 2))->valueString);

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
            Shouter_shoutExpectedToken(cTokenP, BIT_TYPE);
            (*currentTokenHandle)++;
            continue;
        }
        
    }
    (*currentTokenHandle)++;
    return SUCCESS;

}

static bool parseMethodBody_(TokenHandler_t** currentTokenHandle, MethodObjectHandle_t methodHandle)
{

    if(!BodyParser_initialize(&methodHandle->body))
    {
        Log_e(TAG, "Failed to initialize body parser for method \'%s\'", methodHandle->methodName);
        return ERROR;
    }

    if(cTokenType == BRACKET_START)
    {

        (*currentTokenHandle)++;
        // parsing scope
        if(!BodyParser_parseScope(&methodHandle->body, currentTokenHandle))
        {
            Log_e(TAG, "Failed to parse scope for method \'%s\'", methodHandle->methodName);
            return ERROR;
        }

        
    }else
    {
        Shouter_shoutExpectedToken(cTokenP, BRACKET_START);
    }
    
    return SUCCESS;
}