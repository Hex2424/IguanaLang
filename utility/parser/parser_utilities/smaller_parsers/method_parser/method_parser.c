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
#include "../../../parser.h"
////////////////////////////////
// DEFINES
#define cTokenP (**currentTokenHandle)
#define cTokenType cTokenP -> tokenType
#define cTokenIncrement (*currentTokenHandle)++
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

// TODO: make parser contain currentTokenHandle to prevent it always pass through parameters
inline bool MethodParser_parseMethod(TokenHandler_t** currentTokenHandle, ParserHandle_t parser, MainFrameHandle_t root, const Accessibility_t notation)
{
    MethodObjectHandle_t methodHandle;

    ALLOC_CHECK(methodHandle, sizeof(MethodObject_t), ERROR);
    methodHandle->accessType = notation;
    methodHandle->containsBody = false;
    methodHandle->hasInfinityParams = false;

    // setting up method name

    if(!parseMethodReturnVariable_(currentTokenHandle, methodHandle))
    {
        return ERROR;
    }
    
    if(!parseMethodParameters_(currentTokenHandle, methodHandle))
    {
        return ERROR;
    }

    if(!parseMethodBody_(currentTokenHandle, methodHandle))
    {
        return ERROR;
    }


    // if(Hashmap_set(&parser->compiler->AllMethodDeclarations, methodHandle->methodName, methodHandle))
    // {
    //     Shouter_shoutError(cTokenP, "Method \'%s\' is declared several times", methodHandle->methodName);
    //     return ERROR;
    // }


    if(Hashmap_set(&root->methods, methodHandle->methodName, methodHandle))
    {
        Shouter_shoutError(cTokenP, "Method \'%s\' is declared several times", methodHandle->methodName);
        return ERROR;
    }

    return SUCCESS;
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
                cTokenIncrement;
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
                cTokenIncrement;
                continue;
            }else 
            if(cTokenType == BRACKET_ROUND_END)
            {
                break;
            }else
            {
                Shouter_shoutExpectedToken(cTokenP, BRACKET_ROUND_END);
                cTokenIncrement;
                continue;
            }

        }else
        if(cTokenType == BRACKET_ROUND_END)
        {
            break;
        }else if(cTokenType == DOT_SYMBOL)
        {
            cTokenIncrement;
            
            // handling unlimited parameters '...'

            if(cTokenType != DOT_SYMBOL)
            {
                Shouter_shoutError(cTokenP, "expected declaration specifiers or ‘...’ before ‘.’ token");
            }
            
            cTokenIncrement;

            if(cTokenType != DOT_SYMBOL)
            {
                Shouter_shoutError(cTokenP, "expected declaration specifiers or ‘...’ before ‘.’ token");
            }

            cTokenIncrement;

            if(cTokenType != BRACKET_ROUND_END)
            {
                if(cTokenType != COMMA)
                {
                    Shouter_shoutExpectedToken(cTokenP, BRACKET_ROUND_END);
                    break;
                }else
                {
                    Shouter_shoutError(cTokenP, "Infinity params '...' should be declared as end parameter");
                    cTokenIncrement;
                }
                
            }
            // valid infinity parameters syntac acquired
            methodHandle->hasInfinityParams = true;

            
        }
        else
        {
            Shouter_shoutExpectedToken(cTokenP, BIT_TYPE);
            cTokenIncrement;
            continue;
        }
        
    }
    cTokenIncrement;
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

        cTokenIncrement;
        // parsing scope
        if(!BodyParser_parseScope(&methodHandle->body, currentTokenHandle))
        {
            Log_e(TAG, "Failed to parse scope for method \'%s\'", methodHandle->methodName);
            return ERROR;
        }
        
        methodHandle->containsBody = true;
        
    }else if(cTokenType == SEMICOLON)
    {
        methodHandle->containsBody = false;
    }
    else
    {
        Shouter_shoutExpectedToken(cTokenP, BRACKET_START);
    }
    
    return SUCCESS;
}