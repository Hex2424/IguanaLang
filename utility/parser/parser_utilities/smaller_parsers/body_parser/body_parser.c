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

static inline bool handleDotAccess_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool handleMethodCall_(ExMethodCallHandle_t methodCall, TokenHandler_t** currentTokenHandle, const bool isMethodSelf);
static inline bool handleObjectDeclaration_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline bool handleMethodCallParameterization_(MethodObjectHandle_t methodHandle, TokenHandler_t** currentTokenHandle);
static bool handleOperator_(QueueHandle_t expressionQueue, TokenHandler_t** currentTokenHandle);
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
        }else if(cTokenType == SEMICOLON)
        {
            (*currentTokenHandle)++;
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
            expression = malloc(sizeof(Expression_t));
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
            (*currentTokenHandle)++;
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
            (*currentTokenHandle)++;
        }
        

    }
    
    
    return SUCCESS;
}

static bool handleOperator_(QueueHandle_t expressionQueue, TokenHandler_t** currentTokenHandle)
{
    TokenHandler_t checkerTokenHandle = NULL;
    bool nextElementIsExpression = false;

    checkerTokenHandle = (*currentTokenHandle) + 1; // TODO make tokens better way not ptrptrptr, need Object constructor
    
    nextElementIsExpression = isTokenExpression_(&checkerTokenHandle);

    // implement double operators (++,--,==);
    
    if(cTokenType == SEMICOLON)
    {

        (*currentTokenHandle)++;
        return SUCCESS;
    }

    if(!nextElementIsExpression)
    {
        Shouter_shoutError(cTokenP, "Expected ; after expression");
        (*currentTokenHandle)++;
        return SUCCESS;
    }

    // Handling specific operator Actions
    OperatorHandle_t operator;
    operator = malloc(sizeof(Operator_t));
    ALLOC_CHECK(operator, ERROR);

    operator->operatorTokenType = cTokenP->tokenType;

    Queue_enqueue(expressionQueue, operator);

    (*currentTokenHandle)++;
    if(!handleExpression_(expressionQueue, currentTokenHandle))
    {
        Log_e(TAG, "Failed to handle expression '%s'", (*checkerTokenHandle).valueString);
        return ERROR;
    }
    
    return SUCCESS;
}


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