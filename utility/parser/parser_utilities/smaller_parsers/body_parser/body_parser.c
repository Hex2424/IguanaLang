/**
 * @file body_parser.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-11-12
 */

#include "body_parser.h"
#include "../../../../misc/safety_macros.h"
#include "../../../../parser/parser_utilities/global_parser_utility.h"
#include "../../../../parser/parser_utilities/compiler_messages.h"
#include "../../../../parser/structures/expression/expressions.h"

////////////////////////////////
// DEFINES



#define cTokenP (**currentTokenHandle)
#define cTokenType cTokenP -> tokenType


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "BODY_PARSER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS

static inline bool handleKeywordInteger_(LocalScopeObjectHandle_t variablesHashmap, TokenHandler_t** currentTokenHandle);
static inline bool handleKeywordNaming_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline bool handleDotAccess_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool handleMethodCall_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline bool handleObjectDeclaration_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline bool handleMethodCallParameterization_(MethodObjectHandle_t methodHandle, TokenHandler_t** currentTokenHandle);
////////////////////////////////
// IMPLEMENTATION


bool BodyParser_parseScope(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{

    while (cTokenType != BRACKET_END)
    {
        if(cTokenType == INTEGER_TYPE)
        {
            // ignore for now
        }else if(cTokenType == NAMING)
        {
            (*currentTokenHandle)++;
            handleKeywordNaming_(currentTokenHandle, scopeBody);
        }


    }
    
    return SUCCESS;
}

bool BodyParser_initialize(LocalScopeObjectHandle_t scopeBody)
{
    if(!Hashmap_create(&scopeBody->localVariables, NULL))
    {
        Log_e(TAG, "Failed to initialize BodyParser variables hashmap");
        return ERROR;
    }

    if(!Vector_create(&scopeBody->expressions, NULL))
    {
        Log_e(TAG, "Failed to initialize BodyParser expressions vector");
        return ERROR;
    }

    return SUCCESS;
}

static bool handleKeywordNaming_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    if(cTokenType == NAMING)
    {
        // object declaration
        handleObjectDeclaration_(scopeBody, currentTokenHandle);    // Object object;
    }else if(cTokenType == BRACKET_ROUND_START)
    {
        // method call identified
        (*currentTokenHandle)--;
        handleMethodCall_(scopeBody, currentTokenHandle);           // methodCall();

    }else if(cTokenType == DOT_SYMBOL)                              // object.methodCall();
    {
        (*currentTokenHandle)++;
        handleDotAccess_(scopeBody, currentTokenHandle);

    }else
    {
        Shouter_shoutUnrecognizedToken(cTokenP);
        return SUCCESS;
    }

    if(cTokenType != SEMICOLON)
    {
        Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
        (*currentTokenHandle)++;   
    }

    return SUCCESS;

}

static inline bool handleDotAccess_(LocalScopeObjectHandle_t scopeBody,TokenHandler_t** currentTokenHandle)
{
    if(cTokenType == NAMING)
    {
        (*currentTokenHandle)++;
        if(cTokenType == BRACKET_ROUND_START)
        {
            (*currentTokenHandle)--;
            handleMethodCall_(scopeBody, currentTokenHandle);

        }else
        {
            // variable access
        }
        
    }else
    {
        Shouter_shoutUnrecognizedToken(cTokenP);
    }
    
}


static bool handleMethodCall_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    // TODO: method existance check

    ExMethodCallHandle_t methodCall;


    methodCall = malloc(sizeof(MethodObject_t));
    ALLOC_CHECK(methodCall, ERROR);

    methodCall->method.methodName = cTokenP->valueString;

    methodCall->method.parameters = malloc(sizeof(Vector_t));
    ALLOC_CHECK(methodCall->method.parameters, ERROR);

    if(!Vector_create(methodCall->method.parameters, NULL))
    {
        return ERROR;
    }

    (*currentTokenHandle)++; 
    if(!handleMethodCallParameterization_(&methodCall->method, currentTokenHandle))
    {
        return ERROR;
    }

    // if(cTokenType != SEMICOLON)
    // {

    // }

    if(!Vector_append(&scopeBody->expressions, methodCall))
    {
        return ERROR;
    }



    return SUCCESS;
}

static inline bool handleMethodCallParameterization_(MethodObjectHandle_t methodHandle, TokenHandler_t** currentTokenHandle)
{
    
    if(cTokenType == BRACKET_ROUND_START)
    {
        (*currentTokenHandle)++;
        while (true)
        {
            
        //    if(cTokenType == NAMING)
        //    {
        //         // do necesary checking for existing variable (later expression parsing)

        //    }else if(cTokenType == BRACKET_ROUND_END)
        //    {
        //         // parameters end

        //         break;
        //    }else if(cTokenType == COMMA)
        //    {
        //         if()
        //         {

        //         }
        //    }
            if(cTokenType == BRACKET_ROUND_END)
            {
                break;
            }
           
        }
        

    }else
    {
        Shouter_shoutExpectedToken(cTokenP, BRACKET_ROUND_START);
    }
    (*currentTokenHandle)++;
    return SUCCESS;
    
}

static inline bool handleObjectDeclaration_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    
}


static inline bool handleKeywordInteger_(LocalScopeObjectHandle_t variablesHashmap, TokenHandler_t** currentTokenHandle)
{
    VariableObjectHandle_t variable;

    if(!ParserUtils_tryParseSequence(currentTokenHandle, PATTERN_DECLARE, PATTERN_DECLARE_SIZE))
    {
        return SUCCESS;
    }
    variable = malloc(sizeof(VariableObject_t));
    ALLOC_CHECK(variable, ERROR);

    variable->variableName = (*(*currentTokenHandle - 1))->valueString;
    variable->bitpack = atoi((*(*currentTokenHandle - 2))->valueString);
    variable->assignedVariable = NULL;


    if(cTokenType == SEMICOLON)        
    {
        variable->assignedValue = 0;

        if(!Hashmap_putEntry(variablesHashmap, variable->variableName, variable))
        {
            Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->variableName);
            return ERROR;
        }
        
    }else
    if(cTokenType == EQUAL)             // checking for assignable declaration
    {
        (*currentTokenHandle)++;
        if(cTokenType = NUMBER_VALUE)   // assignableValue
        {
            NULL_GUARD(cTokenP->valueString, ERROR, Log_e(TAG, "Cannot parse token value cause its NULL"));

            variable->assignedValue = atoll(cTokenP->valueString);
            (*currentTokenHandle)++;

            if(cTokenType == SEMICOLON)
            {

                if(!Hashmap_putEntry(variablesHashmap, variable->variableName, variable))
                {
                    Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->variableName);
                    return ERROR;
                }

            }else
            {
                Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
            }

        }else
        {
            Shouter_shoutError(cTokenP, "To variable can be assigned constant number or other variable only");
        }

    }else
    {
        Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
    }

    return SUCCESS;
}