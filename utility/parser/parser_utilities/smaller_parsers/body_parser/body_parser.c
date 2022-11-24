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
#define tokenOffsetType(offset) (*((*currentTokenHandle) + 1))->tokenType

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
static bool queueAppendExprObject_(QueueHandle_t expressionQueue, const ExpressionType_t expressionType, void* object);
static inline bool handleMethodCallParameterization_(MethodObjectHandle_t methodHandle, TokenHandler_t** currentTokenHandle);
static bool handleOperator_(QueueHandle_t expressionQueue, TokenHandler_t** currentTokenHandle);
static bool isTokenOperator_(TokenHandler_t** currentTokenHandle);
static bool isTokenExpression_(TokenHandler_t** currentTokenHandle);
static bool handleNaming_(QueueHandle_t expressionQueue, TokenHandler_t** currentTokenHandle);
static bool handleOperations_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool handleExpression_(LocalScopeObjectHandle_t localScope,QueueHandle_t expressions, TokenHandler_t** currentTokenHandle);
static bool parseExpressionLine_(LocalScopeObjectHandle_t localScope,QueueHandle_t expressions, TokenHandler_t** currentTokenHandle);
static bool handleBitType_(LocalScopeObjectHandle_t scopeBody, QueueHandle_t expressions, TokenHandler_t** currentTokenHandle);
////////////////////////////////
// IMPLEMENTATION


bool BodyParser_parseScope(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    QueueHandle_t expressionQueue;

    while (cTokenType != BRACKET_END)
    {

        expressionQueue = malloc(sizeof(Queue_t));
        ALLOC_CHECK(expressionQueue, ERROR);
        
        if(!Queue_create(expressionQueue))
        {
            Log_e(TAG, "Failed to create expression Queue");
            return ERROR;
        }

        if(!parseExpressionLine_(scopeBody, expressionQueue, currentTokenHandle))
        {
            Log_e(TAG, "Failed to parse expression sequence");
            return ERROR;
        }

        if(expressionQueue->count != 0)
        {
            if(!Vector_append(&scopeBody->expressions, expressionQueue))
            {
                Log_e(TAG, "Failed to append expression Queue to expressions vector");
                return ERROR;
            }
        }else
        {
            // deallocating unecessary queue
            Queue_destroy(expressionQueue);
            free(expressionQueue);
        }
        (*currentTokenHandle)++;
        // if(cTokenType != BRACKET_END)
        // {
        //     (*currentTokenHandle)++;
        // }
        
    
    }
    
    return SUCCESS;
}

static inline bool parseExpressionLine_(LocalScopeObjectHandle_t localScope, QueueHandle_t expressions, TokenHandler_t** currentTokenHandle)
{
    bool expectedExpresion;
    expectedExpresion = true;
    
    while (cTokenType != SEMICOLON && cTokenType != BRACKET_END)
    {
        if(isTokenExpression_(currentTokenHandle))
        {
            if(expectedExpresion)
            {
                if(!handleExpression_(localScope, expressions, currentTokenHandle))
                {
                    Log_e(TAG, "Failed to parse expression");
                    return ERROR;
                }
            }else
            {
                Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
                break;
            }
            
            expectedExpresion = !expectedExpresion;

        }else if(isTokenOperator_(currentTokenHandle))
        {
            if(!expectedExpresion)
            {
                if(!handleOperator_(expressions, currentTokenHandle))
                {
                    Log_e(TAG, "Failed to parse operator");
                    return ERROR;
                }
            }else
            {
                Shouter_shoutError(cTokenP, "Expected expression after operator \'%s\'", cTokenP->valueString);
                break;
            }
            
            expectedExpresion = !expectedExpresion;
            
        }

        if(expectedExpresion && (tokenOffsetType(1) == SEMICOLON))
        {
            Shouter_shoutError(cTokenP, "Expected expression after operator \'%s\'", cTokenP->valueString);
        }

        (*currentTokenHandle)++;
    }
    Log_d(TAG, "Finished expression line");
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

static bool handleBitType_(LocalScopeObjectHandle_t scopeBody, QueueHandle_t expressions, TokenHandler_t** currentTokenHandle)
{
    VariableObjectHandle_t variable;
    variable = malloc(sizeof(VariableObject_t));
    ALLOC_CHECK(variable, ERROR);

    (*currentTokenHandle)++;

    if(cTokenType == COLON)
    {
        (*currentTokenHandle)++;

        if(cTokenType == NUMBER_VALUE)
        {
            variable->bitpack = atoi(cTokenP->valueString);
            (*currentTokenHandle)++;

            if(cTokenType == NAMING)
            {
                // no assigning
                variable->variableName = cTokenP->valueString;
            
            }else if(cTokenType == BRACKET_ROUND_START)
            {
                // with assigning
                (*currentTokenHandle)++;
                if(cTokenType == NUMBER_VALUE)
                {
                    variable->assignedValue = cTokenP->valueString;
                    (*currentTokenHandle)++;

                    if(cTokenType == BRACKET_ROUND_END)
                    {
                        (*currentTokenHandle)++;
                        if(cTokenType == NAMING)
                        {
                            variable->variableName = cTokenP->valueString;
                            if(Hashmap_getEntry(&scopeBody->localVariables, variable->variableName) == NULL)
                            {
                                if(!Hashmap_putEntry(&scopeBody->localVariables, variable->variableName, variable))
                                {
                                    Log_e(TAG, "For some reason hashmap entry cannot be putted");
                                    return ERROR;
                                }

                                if(!queueAppendExprObject_(expressions, VARIABLE_NAME, variable->variableName))
                                {
                                    Log_e(TAG, "Failed to put variable name to expressions");
                                    return ERROR;
                                }

                            }else
                            {
                                Shouter_shoutError(cTokenP, "Variable '%s' is already declared", variable->variableName);   
                            }

                        }else
                        {
                            Shouter_shoutError(cTokenP, "Expected variable name after constructor");
                        }
                        
                    }else
                    {
                        Shouter_shoutExpectedToken(cTokenP, BRACKET_ROUND_END);
                    }
                    
                }else
                {
                    Shouter_shoutError(cTokenP, "Expected Constructor parameter, but found this: %s", cTokenP->valueString);
                }
                
            }else
            {
                Shouter_shoutError(cTokenP, "Expected Variable constructutor or variable name after bitpack value");
            }

        }else
        {
            Shouter_shoutError(cTokenP, "After colon ':' need specify constant bitpack field");
        }

        }else
        {
            Shouter_shoutExpectedToken(cTokenP, COLON);
        }

    return SUCCESS;
    
}

static bool handleOperations_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    return SUCCESS;
}

static bool handleNaming_(QueueHandle_t expressionQueue, TokenHandler_t** currentTokenHandle)
{
    TokenHandler_t currentNamingToken;
    while (cTokenType != SEMICOLON)
    {
        // if(tokenOffsetType(1) == DOT_SYMBOL)
        // {
        //     (*currentTokenHandle)++;
        //     if(tokenOffsetType(1) != NAMING)
        //     {
        //         Shouter_shoutError(cTokenP, "With '.' access you can only access object public variable or methods");
        //         expressionQueue->count = 0;
        //         break;
        //     }
        // }else 

        if(tokenOffsetType(1) == BRACKET_ROUND_START)
        {
            ExMethodCallHandle_t methodHandle;

            methodHandle = malloc(sizeof(ExMethodCall_t));
            ALLOC_CHECK(methodHandle, ERROR);

            if(!handleMethodCall_(methodHandle, currentTokenHandle, true))
            {
                Log_e(TAG, "Failed to handle method parsing");
                return ERROR;
            }

            if(!queueAppendExprObject_(expressionQueue, METHOD_CALL, methodHandle))
            {
                Log_e(TAG, "Failed to append expression object");
                return ERROR;
            }

        }else
        {
            if(cTokenType == NAMING)
            {
                if(!queueAppendExprObject_(expressionQueue, VARIABLE_NAME, cTokenP->valueString))
                {
                    Log_e(TAG, "Failed to append expression object");
                    return ERROR;
                }
            }
    
            break;
        }
        
        
    }

    return SUCCESS;
}


static bool handleExpression_(LocalScopeObjectHandle_t localScope,QueueHandle_t expressions, TokenHandler_t** currentTokenHandle)
{

    if(cTokenType == NAMING || cTokenType == THIS)
    {

        if(!handleNaming_(expressions, currentTokenHandle))
        {
            Log_e(TAG, "Failed parse naming");
            return ERROR;
        }
        
    }else if(cTokenType == NUMBER_VALUE)
    {
        ConstantNumberHandle_t constantValue;

        constantValue = malloc(sizeof(ConstantNumber_t));
        ALLOC_CHECK(constantValue, ERROR);

        constantValue->valueAsString = cTokenP->valueString;

        if(!queueAppendExprObject_(expressions, CONSTANT_NUMBER, constantValue))
        {
            Log_e(TAG, "Failed append expression object to queue");
            return ERROR;
        }
        
    }else if(cTokenType == BIT_TYPE)
    {
        if(!handleBitType_(localScope, expressions, currentTokenHandle))
        {   
            Log_e(TAG, "Failed to handle BIT Type declaration");
            return ERROR;
        }
    }else
    {
        Shouter_shoutError(cTokenP, "Expected expression and found '%s'", cTokenP->valueString);
    }
    
    
    return SUCCESS;
}

static bool queueAppendExprObject_(QueueHandle_t expressionQueue, const ExpressionType_t expressionType, void* object)
{
    ExpressionHandle_t expression;
    expression = malloc(sizeof(Expression_t));
    ALLOC_CHECK(expression, ERROR);

    expression->type = expressionType;
    expression->expressionObject = object;
    char* ss = object;
    Log_i(TAG, "Expression contains object type%d", expression->type);

    Queue_enqueue(expressionQueue, expression);

    return SUCCESS;
}


static bool handleOperator_(QueueHandle_t expressionQueue, TokenHandler_t** currentTokenHandle)
{

    // Handling specific operator Actions

    OperatorHandle_t operator;
    operator = malloc(sizeof(Operator_t));
    ALLOC_CHECK(operator, ERROR);

    operator->operatorTokenType = cTokenP->tokenType;

    if(!queueAppendExprObject_(expressionQueue, OPERATOR, operator))
    {
        Log_e(TAG, "Failed to append to queue operator object");
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