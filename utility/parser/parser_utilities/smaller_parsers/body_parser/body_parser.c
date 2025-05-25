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
#include <safety_macros.h>
#include "../../../../parser/parser_utilities/global_parser_utility.h"
#include "../../../../parser/parser_utilities/compiler_messages.h"
#include "../../../../parser/parser_utilities/smaller_parsers/var_parser/var_parser.h"
#include "../../../../parser/parser_utilities/post_parsing_utility/bitfit.h"

#include "../../../../parser/structures/expression/expressions.h"
#include <dstack.h>
////////////////////////////////
// DEFINES



#define cTokenP (**currentTokenHandle)
#define cTokenType cTokenP -> tokenType
#define tokenOffset(offset) (*((*currentTokenHandle) + offset))

////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "BODY_PARSER";

////////////////////////////////
// PRIVATE TYPES

////////////////////////////////
// PRIVATE METHODS

// static inline bool handleDotAccess_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool handleMethodCall_(LocalScopeObjectHandle_t localScopeBody,
    ExMethodCallHandle_t methodCall,
    TokenHandler_t** currentTokenHandle,
    const VariableObjectHandle_t caller);
static inline bool handleObjectDeclaration_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline bool handleMethodCallParameterization_(LocalScopeObjectHandle_t localScopeBody, VectorHandler_t parameters, TokenHandler_t** currentTokenHandle);
static bool handleOperator_(ExpElementHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static bool isTokenOperator_(TokenHandler_t token);
static bool handleNaming_(LocalScopeObjectHandle_t localScopeBody, ExpElementHandle_t symbol, TokenHandler_t** currentTokenHandle, const BitpackSize_t castBitSize, const char* castFileType);
// static bool handleOperations_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool parseExpressionLine_(LocalScopeObjectHandle_t localScope, ExpHandle_t expression, TokenHandler_t** currentTokenHandle,  TokenHandler_t* expressionEndToken);
static bool isSymbolsLegalToExistTogether_(const ExpElementHandle_t firstSymbol, const ExpElementHandle_t secondSymbol);
static inline bool parseVariableInstance_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline int32_t expressionPrecedence_(ExpElementHandle_t symbol);
static inline bool parseSymbolExpression_(LocalScopeObjectHandle_t scopeBody, ExpElementHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static bool handleNumeric_(LocalScopeObjectHandle_t scopeBody, ExpElementHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static bool handlePostASTMethodCall_(ExMethodCallHandle_t methodCall);
static VariableObjectHandle_t searchVariableNameAcrossScopes(LocalScopeObjectHandle_t localScopeBody, const char* varName);
static VariableObjectHandle_t createUnknownVar_(char* notFoundVarName);
////////////////////////////////
// IMPLEMENTATION


static inline int32_t expressionPrecedence_(ExpElementHandle_t symbol) 
{
    int32_t prec = -1;

    if(ExpElement_getType(symbol) == EXP_OPERATOR)
    {
        OperatorType_t operatorType = (OperatorType_t) ExpElement_getObject(symbol);

        switch (operatorType)
        {
            case OP_BIN_NOT:
            {
                prec = 5; // Unary NOT (if used as prefix op)
            }break;

            case OP_CAST:
            {
                prec = 4;
            }break;

            case OP_MULTIPLY:
            case OP_DIVIDE:
            case OP_MODULUS:
            {
                prec = 3;
            }break;

            case OP_PLUS:
            case OP_MINUS:
            {
                prec = 2;
            }break;

            case OP_BIN_AND:
            case OP_BIN_XOR:
            case OP_BIN_OR:
            {
                prec = 1;
            }break;

            case OP_SET:
            {
                prec = 0;
            }break;

            
            default: prec = -1; break;
        }
        
    }

    return prec;
}

bool BodyParser_parseScope(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    while ((cTokenType != BRACKET_END) && (cTokenType != END_FILE))
    {

        switch (cTokenType)
        {
            // TODO: handling ERRORS
            case BIT_TYPE: parseVariableInstance_(scopeBody, currentTokenHandle); break;

            case NAMING:
            case NUMBER_VALUE:
            case BRACKET_ROUND_START: 
            // case THIS:
            case OPERATOR_NOT:
            {
                ExpHandle_t expression = Expression_createDynamic(SIMPLE_LINE);

                NULL_GUARD(expression, ERROR, Log_e(TAG, "Failed to create / allocate expression Vector"));

                TokenHandler_t* startExpressionPtr = (*currentTokenHandle);
                ParserUtils_skipUntil(currentTokenHandle, (TokenType_t[]){SEMICOLON, BRACKET_END, BRACKET_START}, 3);
                TokenHandler_t* endExpressionPtr = (*currentTokenHandle);
                
                (*currentTokenHandle) = startExpressionPtr;

                if(!parseExpressionLine_(scopeBody, expression, currentTokenHandle, endExpressionPtr))
                {
                    Log_e(TAG, "Failed to parse expression line");
                    return ERROR;
                }

                if(cTokenType != SEMICOLON)
                {
                    Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
                    (*currentTokenHandle)--;
                }else
                {
                    if(!Vector_append(&scopeBody->scopeElementsList, expression))
                    {
                        Log_e(TAG, "Failed to append expression line to expression lines list");
                        return ERROR;
                    }
                }

            }break;


            case SEMICOLON: break;

            default: Shouter_shoutUnrecognizedToken(cTokenP);break;
        }
        
        (*currentTokenHandle)++;
    
    }

    return SUCCESS;
}


static inline bool parseVariableInstance_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle)
{
    VariableObjectHandle_t variable;

    ALLOC_CHECK(variable, sizeof(VariableObject_t), ERROR);

    if(!VarParser_parseVariable(currentTokenHandle, variable))
    {
        return ERROR;
    }

    // assigning object / class local variables array scope
    variable->scopeName = LOCAL_VAR_REGION_NAME;
    
    Log_d(TAG, "Parsed variable instance: %s", variable->objectName);

    (*currentTokenHandle)++;

    if(cTokenType == SEMICOLON)
    {
        if(Hashmap_set(&scopeBody->localVariables, variable->objectName, variable))
        {
            Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->objectName);
            return ERROR;
        }
        
    }else if(cTokenType == EQUAL)
    {
        // it is insta initialized
        // getting back to name of variable, so it gets added to expression
        if(Hashmap_set(&scopeBody->localVariables, variable->objectName, variable))
        {
            Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->objectName);
            return ERROR;
        }

        (*currentTokenHandle) -= 2;
        
    }else
    {
        Shouter_shoutUnrecognizedToken(cTokenP);
        ParserUtils_skipUntil(currentTokenHandle, (TokenType_t[]){SEMICOLON}, 1);
    }


    return SUCCESS;
}

static bool parseExpressionLine_(LocalScopeObjectHandle_t localScope, ExpHandle_t expression, TokenHandler_t** currentTokenHandle,  TokenHandler_t* expressionEndToken)
{
    ExpElementHandle_t lastSymbol = NULL;
    ExpElementHandle_t currentSymbol = NULL;

    DynamicStack_t symbolStack;

    if(!Stack_create(&symbolStack))
    {
        Log_e(TAG, "Couldn't create symbols parsing stack");
        return ERROR;
    }

    while (*currentTokenHandle != expressionEndToken)
    {
        ExpElementHandle_t symbol;

        ALLOC_CHECK(symbol, sizeof(ExpElement_t), ERROR);
        
        Log_d(TAG, "Parsing symbol in expression line %s", cTokenP->valueString);
        if(!parseSymbolExpression_(localScope, symbol, currentTokenHandle))
        {
            Shouter_shoutError(cTokenP, "Symbol Parse error \'%s\'", cTokenP->valueString);
            ParserUtils_skipUntil(currentTokenHandle, (TokenType_t[]){SEMICOLON, BRACKET_END, BRACKET_START}, 3);
            free(symbol);
            return SUCCESS;
        }
        
        lastSymbol = currentSymbol;
        currentSymbol = symbol;

        if(!isSymbolsLegalToExistTogether_(lastSymbol, currentSymbol))
        {
            Shouter_shoutError(cTokenP, "Illegal token to use after prev token");
            ParserUtils_skipUntil(currentTokenHandle, (TokenType_t[]){SEMICOLON, BRACKET_END, BRACKET_START}, 3);
            return SUCCESS;
        }

        // If the scanned character is 
        // an operand, add it to the output string.
        if (ExpElement_isSymbolOperand(symbol))
        {
            if(!Expression_addElement(expression, symbol))
            {
                Log_e(TAG, "Failed to append expression element to expression");
                return ERROR;
            }
        }

        // If the scanned character is
        // an ‘(‘, push it to the stack.
        else if (symbol->type == EXP_PARENTHESES_LEFT)
        {
            if(!Stack_push(&symbolStack, symbol))
            {
                Log_e(TAG, "Failed to push symbol to stack");
                return ERROR;
            }
        }

        // If the scanned character is an ‘)’,
        // pop and add to the output string from the stack 
        // until an ‘(‘ is encountered.
        else if (symbol->type == EXP_PARENTHESES_RIGHT) 
        {
            while (!Stack_isEmpty(&symbolStack) && (((ExpElementHandle_t) Stack_peek(&symbolStack))->type != EXP_PARENTHESES_LEFT)) 
            {
                if(!Expression_addElement(expression, Stack_pop(&symbolStack)))
                {
                    Log_e(TAG, "Failed to append expression element to expression");
                    return ERROR;
                }
            }
            Stack_pop(&symbolStack);
        }

        // If an operator is scanned
        else 
        {
            while (!Stack_isEmpty(&symbolStack) && (expressionPrecedence_(symbol) <= expressionPrecedence_(Stack_peek(&symbolStack)))) 
            {
                if(!Expression_addElement(expression, Stack_pop(&symbolStack)))
                {
                    Log_e(TAG, "Failed to append expression element to expression");
                    return ERROR;
                }
            }
            if(!Stack_push(&symbolStack, symbol))
            {
                Log_e(TAG, "Failed to push symbol to stack");
                return ERROR;
            }
        }
    

        (*currentTokenHandle)++;
    }

    currentSymbol = NULL;

    // Pop all the remaining elements from the stack
    while (!Stack_isEmpty(&symbolStack)) 
    {
        if(!Expression_addElement(expression, Stack_pop(&symbolStack)))
        {
            Log_e(TAG, "Failed to append expression element to expression");
            return ERROR;
        }
    }

    ExpIterator_t firstIt = Expression_iteratorFirst(expression);
    ExpIterator_t lastIt = Expression_iteratorLast(expression);

    for(ExpIterator_t iterator = firstIt; iterator < lastIt; iterator++)
    {
        ExpElementHandle_t element = *iterator;
        ExpElementType_t type = ExpElement_getType(element);

        if (type == EXP_CONST_NUMBER)
        {
            Log_i(TAG, "symbol: NUM(%d)", (AssignValue_t) ExpElement_getObject(element));
        }else if(type == EXP_OPERATOR)
        {
            Log_i(TAG, "symbol: OP: %d", (OperatorType_t) ExpElement_getObject(element));
        }else if(type == EXP_VARIABLE)
        {
            VariableObjectHandle_t var = (VariableObjectHandle_t) ExpElement_getObject(element);
            NULL_GUARD(var, ERROR, Log_e(TAG, "Null element in expression rely"));
            
            Log_i(TAG, "symbol: bit:%u(%s) %s", var->bitpack, var->castedFile, var->objectName);
        }else if(type == EXP_METHOD_CALL)
        {
            ExMethodCallHandle_t var = (ExMethodCallHandle_t) ExpElement_getObject(element);

            NULL_GUARD(var, ERROR, Log_e(TAG, "Null element in expression rely"));
            if (var->caller != NULL)
            {
                Log_i(TAG, "symbol: method: %s.%s", var->caller->objectName, var->name);
            }else
            {
                Log_i(TAG, "symbol: method: this.%s", var->name);
            }
            
        }
        
    }
    
    Stack_destroy(&symbolStack);
    
    return SUCCESS;
}


bool BodyParser_initialize(LocalScopeObjectHandle_t scopeBody)
{
    InitialSettings_t initialSettingExpressions;

    initialSettingExpressions.containsVectors = true;
    initialSettingExpressions.initialSize = 10;
    initialSettingExpressions.expandableConstant = (1.0f / 2.0f);

    if(!Hashmap_new(&scopeBody->localVariables, 5))
    {
        Log_e(TAG, "Failed to initialize BodyParser variables hashmap");
        return ERROR;
    }

    if(!Vector_create(&scopeBody->scopeElementsList, &initialSettingExpressions))
    {
        Log_e(TAG, "Failed to initialize BodyParser expressions vector");
        return ERROR;
    }

    return SUCCESS;
}

static inline bool parseSymbolExpression_(LocalScopeObjectHandle_t scopeBody, ExpElementHandle_t symbol, TokenHandler_t** currentTokenHandle)
{
    
    if(isTokenOperator_(cTokenP))
    {
        Log_d(TAG, "parsing operator: %s", cTokenP->valueString);

        if(!handleOperator_(symbol, currentTokenHandle))
        {
            Log_e(TAG, "Operator parsing malfunction");
            return ERROR;
        }
    }
    else
    {
        switch (cTokenType)
        {
            case NAMING: 
            {
                Log_d(TAG, "parsing naming: %s", cTokenP->valueString);

                if(!handleNaming_(scopeBody, symbol, currentTokenHandle, 0, NULL))
                {
                    Log_e(TAG, "Error happened while handling variable%s in expression", cTokenP->valueString);
                    return ERROR;
                }
             
            }break;

            case NUMBER_VALUE:
            {
                Log_d(TAG, "parsing number: %s", cTokenP->valueString);

                if(!handleNumeric_(scopeBody, symbol, currentTokenHandle))
                {
                    Log_e(TAG, "Error happened while handling number%s in expression", cTokenP->valueString);
                    return ERROR;
                }
            }break;break;

            case BRACKET_ROUND_START:
            {
                if(!ExpElement_setType(symbol, EXP_PARENTHESES_LEFT))
                {
                    Log_e(TAG, "Expression element set type failed EXP_PARENTHESES_LEFT");
                    return ERROR;
                }
            }break;
            case BRACKET_ROUND_END:
            {
                if(!ExpElement_setType(symbol, EXP_PARENTHESES_RIGHT))
                {
                    Log_e(TAG, "Expression element set type failed EXP_PARENTHESES_RIGHT");
                    return ERROR;
                }
            }break;
            
            default:
            {
                Log_e(TAG, "Unrecognised token type detected %d", cTokenType);
            }return ERROR;
        }
    }

    return SUCCESS;

}

static bool handleNumeric_(LocalScopeObjectHandle_t scopeBody, ExpElementHandle_t symbol, TokenHandler_t** currentTokenHandle)
{
    AssignValue_t number;
    char *end;
    // in future may need more logic according to this
    // symbolHandle->type = EXP_CONST_NUMBER;
    // Parsing number
    number = strtol(cTokenP->valueString, &end, 10);

    if(!ExpElement_set(symbol, EXP_CONST_NUMBER, (void*) (uintptr_t) number))
    {
        Log_e(TAG, "Failed to set symbol params");
        return ERROR;
    }

    return SUCCESS;
}

static bool handleOperator_(ExpElementHandle_t symbolHandle, TokenHandler_t** currentTokenHandle)
{

    void* operatorObject;
    if(!ExpElement_setType(symbolHandle, EXP_OPERATOR))
    {
        Log_e(TAG, "Failed to set type operator");
        return ERROR;
    }

    switch (cTokenType)
    {
        // TODO make work with double operators or even bigger like &&
        case OPERATOR_PLUS: operatorObject = (void*) (uintptr_t) (OP_PLUS);break;
        case OPERATOR_MINUS: operatorObject= (void*) (uintptr_t) (OP_MINUS);break;
        case OPERATOR_MODULUS: operatorObject = (void*) (uintptr_t) (OP_MODULUS);break;
        case OPERATOR_MULTIPLY: operatorObject = (void*) (uintptr_t) (OP_MULTIPLY);break;
        case OPERATOR_DIVIDE: operatorObject = (void*) (uintptr_t) (OP_DIVIDE);break;
        case NOT: operatorObject = (void*) (uintptr_t) (OP_NOT);break;
        case OPERATOR_XOR: operatorObject = (void*) (uintptr_t) (OP_BIN_XOR);break;
        case OPERATOR_AND: operatorObject = (void*) (uintptr_t) (OP_BIN_AND);break;
        case OPERATOR_OR: operatorObject = (void*) (uintptr_t) (OP_BIN_OR);break;
        case OPERATOR_NOT: operatorObject = (void*) (uintptr_t) (OP_BIN_NOT);break;
        case EQUAL: operatorObject = (void*) (uintptr_t) (OP_SET);break;
        case COLON: operatorObject = (void*) (uintptr_t) (OP_CAST);break;

        default:
        {
            Log_e(TAG, "Not recognised operator detected: %d", cTokenP->valueString);
        }return ERROR;
    }

    if(!ExpElement_setObject(symbolHandle, operatorObject))
    {
        Log_e(TAG, "Failed to set operator object");
        return ERROR;
    }

    return SUCCESS;
}

static bool handleNaming_(LocalScopeObjectHandle_t localScopeBody, ExpElementHandle_t symbol, TokenHandler_t** currentTokenHandle, const BitpackSize_t castBitSize, const char* castFileType)
{
    // TokenHandler_t currentNamingToken;
    (*currentTokenHandle)++;

    if(cTokenType == BRACKET_ROUND_START)
    {
        ExMethodCallHandle_t methodHandle;

        ALLOC_CHECK(methodHandle, sizeof(ExMethodCall_t), ERROR);

        (*currentTokenHandle)--;
        Log_d(TAG, "Parsing method call: this.%s", cTokenP->valueString);
        
        if(!handleMethodCall_(localScopeBody, methodHandle, currentTokenHandle, NULL))
        {
            Log_e(TAG, "Failed to handle method parsing");
            return ERROR;
        }

        if(!ExpElement_set(symbol, EXP_METHOD_CALL, methodHandle))
        {
            Log_e(TAG, "Failed to add method call to expression element");
            return ERROR;
        }

    }else
    {   
        (*currentTokenHandle)--;

        // Recognised some kind of variable

        const char* varName = cTokenP->valueString;
        Log_d(TAG, "Parsing variable: %s", varName);

        VariableObjectHandle_t foundVariableCorresponding = searchVariableNameAcrossScopes(localScopeBody, varName);
        
        if(foundVariableCorresponding != NULL)
        {    
            // Object call
            if(tokenOffset(1)->tokenType == DOT_SYMBOL)
            {
                (*currentTokenHandle) += 2;
                if(cTokenType == NAMING)
                {
                    (*currentTokenHandle)++;
                    
                    if(cTokenType== BRACKET_ROUND_START)
                    {
                        ExMethodCallHandle_t methodHandle;

                        ALLOC_CHECK(methodHandle, sizeof(ExMethodCall_t), ERROR);

                        (*currentTokenHandle)--;
                        Log_d(TAG, "Parsing method call: %s.%s", foundVariableCorresponding->objectName, cTokenP->valueString);

                        if(!handleMethodCall_(localScopeBody, methodHandle, currentTokenHandle, foundVariableCorresponding))
                        {
                            Log_e(TAG, "Failed to handle method parsing");
                            return ERROR;
                        }

                        if(!ExpElement_set(symbol, EXP_METHOD_CALL, methodHandle))
                        {
                            Log_e(TAG, "Failed to add method call to expression element");
                            return ERROR;
                        }
                    }else
                    {
                        Shouter_shoutExpectedToken(cTokenP, BRACKET_ROUND_START);
                    }
                }else
                {
                    Shouter_shoutError(cTokenP, "Expecting function call after dot");
                }
            }
            else
            {
                if(!ExpElement_set(symbol, EXP_VARIABLE, foundVariableCorresponding))
                {
                    Log_e(TAG, "Failed to add exp variable to expression element");
                    return ERROR;
                }
            }

        }else
        {
            if(!ExpElement_set(symbol, EXP_VARIABLE, createUnknownVar_((char*) varName)))
            {
                Log_e(TAG, "Failed to add exp variable to expression element");
                return ERROR;
            }

            Shouter_shoutError(cTokenP, "Variable '%s' is not declared previously", varName);
        }
 
    }
        
    return SUCCESS;
}

static VariableObjectHandle_t createUnknownVar_(char* notFoundVarName)
{
    VariableObjectHandle_t unknownVar;
    
    ALLOC_CHECK(unknownVar, sizeof(VariableObject_t), NULL);

    unknownVar->belongToGroup = 0;
    unknownVar->bitpack = 0;
    unknownVar->castedFile = NULL;
    unknownVar->posBit = 0;
    unknownVar->scopeName = NULL;
    unknownVar->objectName = notFoundVarName;

    return unknownVar;
}

static VariableObjectHandle_t searchVariableNameAcrossScopes(LocalScopeObjectHandle_t localScopeBody, const char* varName)
{
    VariableObjectHandle_t varFound;
    int found;

    found = Hashmap_find(&localScopeBody->localVariables, varName, strlen(varName));

    if(found)
    {
        return *(localScopeBody->localVariables.value);
    }

    varFound = VarParser_searchVariableInVectorByName(localScopeBody->paramsVarsRef, varName);
    
    if(varFound != NULL)
    {
        return varFound;
    }

    found = Hashmap_find(localScopeBody->objectVarsRef, varName, strlen(varName));
    
    if(found)
    {
        return *(localScopeBody->objectVarsRef->value);
    }

    return NULL;
}



static bool handleMethodCall_(LocalScopeObjectHandle_t localScopeBody,
    ExMethodCallHandle_t methodCall,
    TokenHandler_t** currentTokenHandle,
    const VariableObjectHandle_t caller)
{
    // TODO: method existance check
    
    methodCall->name = cTokenP->valueString;
    methodCall->caller = caller;

    if(!Vector_create(&methodCall->parameters, NULL))
    {
        Log_e(TAG, "Failed to create function call %s params vector", methodCall->name);
        return ERROR;
    }

    (*currentTokenHandle)++; 
    if(!handleMethodCallParameterization_(localScopeBody, &methodCall->parameters, currentTokenHandle))
    {
        Log_e(TAG, "Failed to handle method call parameter parsing");
        return ERROR;
    }

    if(!handlePostASTMethodCall_(methodCall))
    {
        Log_e(TAG, "Failed to handle method call postprocessing");
        return ERROR;
    }

    return SUCCESS;
}

static bool handlePostASTMethodCall_(ExMethodCallHandle_t methodCall)
{
    return SUCCESS;
}


static inline bool handleMethodCallParameterization_(LocalScopeObjectHandle_t localScopeBody, VectorHandler_t parameters, TokenHandler_t** currentTokenHandle)
{
    
    if(cTokenType == BRACKET_ROUND_START)
    {
        (*currentTokenHandle)++;

        uint32_t unclosedRoundBracketsCount = 1;
        TokenHandler_t* startExpressionPtr = (*currentTokenHandle);

        while ((unclosedRoundBracketsCount > 0) && (cTokenType != END_FILE) &&
            (cTokenType != BRACKET_START) &&
            (cTokenType != BRACKET_END) &&
            (cTokenType != NOTATION))
        {
            /* code */
            if(cTokenType == BRACKET_ROUND_END)
            {
                unclosedRoundBracketsCount--;
            }

            if(cTokenType == BRACKET_ROUND_START)
            {
                unclosedRoundBracketsCount++;
            }

            (*currentTokenHandle)++;
        }

        TokenHandler_t* endExpressionFunctionPtr = (*currentTokenHandle) - 1;

        if(startExpressionPtr == endExpressionFunctionPtr)
        {
            (*currentTokenHandle)--;
            return SUCCESS;
        }

        (*currentTokenHandle) = startExpressionPtr;

        if(unclosedRoundBracketsCount > 0)
        {
            Shouter_shoutError(cTokenP, "Unallowed symbol in params list: \'%s\'", cTokenP->valueString);

        }else
        {
            TokenHandler_t* endExpressionPtr;

            startExpressionPtr = (*currentTokenHandle);

            while(true)
            {
                if(cTokenType == BRACKET_ROUND_END)
                {
                    unclosedRoundBracketsCount--;
                }

                if(cTokenType == BRACKET_ROUND_START)
                {
                    unclosedRoundBracketsCount++;
                }

                if(((unclosedRoundBracketsCount == 0) && (cTokenType == COMMA)) || (*currentTokenHandle) == endExpressionFunctionPtr)
                {
                    endExpressionPtr = (*currentTokenHandle);
                    (*currentTokenHandle) = startExpressionPtr;

                    ExpHandle_t expression =  Expression_createDynamic(SIMPLE_LINE);

                    NULL_GUARD(expression, ERROR, Log_e(TAG, "Failed to create / allocate expression Vector"));

                    if(!parseExpressionLine_(localScopeBody, expression, currentTokenHandle, endExpressionPtr))
                    {
                        Log_e(TAG, "Failed to parse expression line");
                        return ERROR;
                    }

                    if(!Vector_append(parameters, expression))
                    {
                        Log_e(TAG, "Failed to append parameter expression to parameters list");
                        return ERROR;
                    }

                    if(endExpressionFunctionPtr == endExpressionPtr)
                    {
                        // Params parsing ended
                        break;
                    }else
                    {
                        (*currentTokenHandle)++;
                        startExpressionPtr = (*currentTokenHandle);
                    }

                }else
                {
                    (*currentTokenHandle)++;
                }
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
    return false;
}

static bool isTokenOperator_(TokenHandler_t token)
{
    return  token->tokenType == OPERATOR_PLUS       ||
            token->tokenType == OPERATOR_MINUS      ||
            token->tokenType == OPERATOR_MODULUS    ||
            token->tokenType == OPERATOR_MULTIPLY   ||
            token->tokenType == OPERATOR_DIVIDE     ||
            token->tokenType == OPERATOR_XOR        ||
            token->tokenType == OPERATOR_AND        ||
            token->tokenType == OPERATOR_OR         ||
            token->tokenType == OPERATOR_NOT        ||
            token->tokenType == COLON               ||
            token->tokenType == EQUAL;             
            // cTokenType == OPERATOR_DIVIDE;

    // TODO: Add binary operators
}

static bool isSymbolsLegalToExistTogether_(const ExpElementHandle_t firstSymbol, const ExpElementHandle_t secondSymbol)
{
    bool legal = false;

    ExpElementType_t firstType;
    ExpElementType_t secondType;

    if(firstSymbol == NULL)
    {
        firstType = EXP_OPERATOR;
    }else
    {
        firstType = ExpElement_getType(firstSymbol);
    }

    if(secondSymbol == NULL)
    {
        secondType = EXP_OPERATOR;
    }else
    {
        secondType = ExpElement_getType(secondSymbol);
    }

    switch (firstType)
    {   
        case EXP_PARENTHESES_RIGHT:
        case EXP_METHOD_CALL:
        case EXP_VARIABLE:
        case EXP_CONST_NUMBER:
        {
            switch (secondType)
            {
                case EXP_PARENTHESES_RIGHT:
                case EXP_OPERATOR:
                {
                    legal = true;
                }break;

                case EXP_VARIABLE:
                case EXP_PARENTHESES_LEFT:
                case EXP_METHOD_CALL:
                case EXP_CONST_NUMBER:
                {
                    legal = false;
                }break;
                
                default:break;
            }
        }break;
    
        case EXP_PARENTHESES_LEFT:
        case EXP_OPERATOR:
        {
            switch (ExpElement_getType(secondSymbol))
            {
                case EXP_PARENTHESES_RIGHT:
                case EXP_OPERATOR:
                {
                    // need handle 1 var operators like NOT
                    legal = false;
                }break;

                case EXP_VARIABLE:
                case EXP_PARENTHESES_LEFT:
                case EXP_METHOD_CALL:
                case EXP_CONST_NUMBER:
                {
                    legal = true;
                }break;
                
                default:break;
            }
        }break;

        default:break;
    }

    return legal;
}