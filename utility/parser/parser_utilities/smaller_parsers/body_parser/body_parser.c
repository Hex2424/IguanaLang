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
#include "../../../../queue/queue.h"
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
// static inline bool handleKeywordNaming_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);

static inline bool handleDotAccess_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool handleMethodCall_(ExMethodCallHandle_t methodCall, TokenHandler_t** currentTokenHandle, const bool isMethodSelf);
static inline bool handleObjectDeclaration_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline bool handleMethodCallParameterization_(MethodObjectHandle_t methodHandle, TokenHandler_t** currentTokenHandle);
static bool handleOperator_(QueueHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool isTokenOperator_(TokenHandler_t** currentTokenHandle);
static bool isTokenExpression_(TokenHandler_t** currentTokenHandle);
static bool handleOperations_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool handleExpression_(QueueHandle_t expressionQueue, TokenHandler_t** currentTokenHandle);
////////////////////////////////
// IMPLEMENTATION


bool BodyParser_parseScope(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
  
    while (true)
    {
        if(isTokenOperator_(currentTokenHandle))
        {

            if(!handleOperator_(scopeBody, currentTokenHandle))
            {
                return ERROR;
            }

        }else if(isTokenExpression_(currentTokenHandle))
        {
            QueueHandle_t expressionQueue;
            expressionQueue = malloc(sizeof(Queue_t));
            ALLOC_CHECK(expressionQueue, ERROR);

            if(!handleExpression_(expressionQueue, currentTokenHandle))
            {
                Log_e(TAG, "Failed to parse expression");
                return ERROR;
            }
            if(!Vector_append(&scopeBody->expressions, expressionQueue))
            {
                Log_e(TAG, "Failed to append expression queue");
                return ERROR;
            }

        }else if(cTokenType == BRACKET_END)
        {
            break;   
        }else
        {
            if(!handleOperations_(scopeBody, currentTokenHandle))
            {
                Log_e(TAG, "Failed to handle operations");
                return ERROR;
            }
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

static bool handleOperations_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    return SUCCESS;
}


static bool handleExpression_(QueueHandle_t expressionQueue, TokenHandler_t** currentTokenHandle)
{

    if(cTokenType == NAMING)
    {
        (*currentTokenHandle)++;
        if(isTokenOperator_(currentTokenHandle))
        {
            if(!handleOperator_(expressionQueue, currentTokenHandle))
            {
                Log_e(TAG, "Failed to handle Operator %s", cTokenP->valueString);
                return ERROR;
            }

        }else if(cTokenType == SEMICOLON)
        {
            (*currentTokenHandle)++;
            return SUCCESS;

        }else if(cTokenType == BRACKET_ROUND_START)
        {
            (*currentTokenHandle)--;

            // Allocating Method Call object
            
            ExpressionHandle_t expression;
            expression = malloc(sizeof(MethodObject_t));
            ALLOC_CHECK(expression, ERROR);

            ExMethodCallHandle_t methodCall;
            methodCall = malloc(sizeof(MethodObject_t));
            ALLOC_CHECK(methodCall, ERROR);

            expression->type = METHOD_CALL;
            expression->expressionObject = methodCall;
            
            if(!handleMethodCall_(methodCall, currentTokenHandle, true))
            {
                Log_e(TAG, "Failed to handle method call parsing \'%s\'", cTokenP->valueString);
                return ERROR;
            }

            Queue_enqueue(expressionQueue, expression);
            if(!handleOperator_(expressionQueue, currentTokenHandle))
            {
                Log_e(TAG, "Failed to handle Operator %s", cTokenP->valueString);
                return ERROR;
            } 
            
        }else if(cTokenP == DOT_SYMBOL)
        {
            // nothing for now
            

        }else
        {
            Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
        }
        
        
    }else if(cTokenType == THIS)
    {

        (*currentTokenHandle)++;

        if(isTokenOperator_(currentTokenHandle))
        {
            if(!handleOperator_(expressionQueue, currentTokenHandle))
            {
                Log_e(TAG, "Failed to handle Operator \'%s\'", cTokenP->valueString);
                return ERROR;
            }

        }else if(cTokenType == SEMICOLON)
        {

            (*currentTokenHandle)++;
            return SUCCESS;

        }else if(cTokenType == DOT_SYMBOL)
        {
            // Calling method inside this object
            (*currentTokenHandle)++;
            if(!handleExpression_(expressionQueue, currentTokenHandle))
            {
                Log_e(TAG, "Failed to handle Expression with token \'%s\'", cTokenP->valueString);
                return ERROR;
            }

        }else
        {
            Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
        }
        

    }
    
    
    return SUCCESS;
}

static bool handleOperator_(QueueHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    if(cTokenType == SEMICOLON)
    {
        (*currentTokenHandle)++;
        return SUCCESS;
    }else
    {
        Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
    }
    

    
    // if(cTokenType == NAMING)
    // {
    //     // object declaration
    //     handleObjectDeclaration_(scopeBody, currentTokenHandle);    // Object object;
    // }else if(cTokenType == BRACKET_ROUND_START)
    // {
    //     // method call identified
    //     (*currentTokenHandle)--;
    //     handleMethodCall_(scopeBody, currentTokenHandle);           // methodCall();

    // }else if(cTokenType == DOT_SYMBOL)                              // object.methodCall();
    // {
    //     (*currentTokenHandle)++;
    //     handleDotAccess_(scopeBody, currentTokenHandle);

    // }else
    // {
    //     Shouter_shoutUnrecognizedToken(cTokenP);
    //     return SUCCESS;
    // }

    // if(cTokenType != SEMICOLON)
    // {
    //     Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
    //     (*currentTokenHandle)++;   
    // }
}

// static bool handleKeywordNaming_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
// {

    

//     return SUCCESS;

// }

// static inline bool handleDotAccess_(LocalScopeObjectHandle_t scopeBody,TokenHandler_t** currentTokenHandle)
// {
//     if(cTokenType == NAMING)
//     {
//         (*currentTokenHandle)++;
//         if(cTokenType == BRACKET_ROUND_START)
//         {
//             (*currentTokenHandle)--;
//             if(!handleMethodCall_(scopeBody, currentTokenHandle))
//             {
//                 return ERROR;
//             }


//         }else
//         {
//             // variable access
//         }
        
//     }else
//     {
//         Shouter_shoutUnrecognizedToken(cTokenP);
//     }
    
// }


static bool handleMethodCall_(ExMethodCallHandle_t methodCall, TokenHandler_t** currentTokenHandle, const bool isMethodSelf)
{
    // TODO: method existance check

    methodCall->method.methodName = cTokenP->valueString;
    methodCall->isMethodSelf = isMethodSelf;
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

static bool isTokenOperator_(TokenHandler_t** currentTokenHandle)
{
    return  cTokenType == OPERATOR_PLUS     ||
            cTokenType == OPERATOR_MINUS    ||
            cTokenType == OPERATOR_MODULUS  ||
            cTokenType == OPERATOR_MULTIPLY ||
            cTokenType == EQUAL             ||
            cTokenType == OPERATOR_DIVIDE;

    // TODO: Add binary operators
}

static bool isTokenExpression_(TokenHandler_t** currentTokenHandle)
{
    return  cTokenType == BIT_TYPE      ||
            cTokenType == NAMING        ||
            cTokenType == NUMBER_VALUE  ||
            cTokenType == THIS          ||
            cTokenType == LITTERAL;


    // TODO: Add binary operators
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