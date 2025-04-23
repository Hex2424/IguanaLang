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
#include <stack.h>
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
static bool handleMethodCall_(ExMethodCallHandle_t methodCall, TokenHandler_t** currentTokenHandle, const bool isMethodSelf);
static inline bool handleObjectDeclaration_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline bool handleMethodCallParameterization_(MethodObjectHandle_t methodHandle, TokenHandler_t** currentTokenHandle);
static bool handleOperator_(ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static bool isTokenOperator_(TokenHandler_t** currentTokenHandle);
static bool isSymbolOperand_(const ExpressionHandle_t symbol);
static bool handleNaming_(LocalScopeObjectHandle_t localScopeBody, ExpressionHandle_t symbol, TokenHandler_t** currentTokenHandle);
// static bool handleOperations_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool parseExpressionLine_(LocalScopeObjectHandle_t localScope, TokenHandler_t** currentTokenHandle);
static bool isSymbolsLegalToExistTogether_(const ExpressionHandle_t firstSymbol, const ExpressionHandle_t secondSymbol);
static inline bool parseVariableInstance_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline int32_t expressionPrecedence_(ExpressionHandle_t symbol);
static inline bool parseSymbolExpression_(LocalScopeObjectHandle_t scopeBody, ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static bool handleNumber_(ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static inline bool postParsingJobsScope_(LocalScopeObjectHandle_t scopeBody);
////////////////////////////////
// IMPLEMENTATION


static inline int32_t expressionPrecedence_(ExpressionHandle_t symbol) 
{
    int32_t prec = -1;

    if(symbol->type == EXP_OPERATOR)
    {
        OperatorType_t operatorType = (OperatorType_t) symbol->expressionObject;

        switch (operatorType)
        {
            case OP_MULTIPLY:
            case OP_DIVIDE:
            {
                prec = 3;
            }break;

            case OP_PLUS:
            case OP_MINUS:
            {
                prec = 2;
            }break;

            case OP_BIN_AND:
            case OP_BIN_OR:
            case OP_BIN_NOT:
            case OP_BIN_XOR:
            case OP_MODULUS:
            case OP_AND:
            case OP_OR:
            case OP_SET:
            {
                prec = 1;
            }break;
            
            default:break;
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
            case OPERATOR_NOT: parseExpressionLine_(scopeBody, currentTokenHandle); break;


            case SEMICOLON: break;

            default: Shouter_shoutUnrecognizedToken(cTokenP);break;
        }
        
        (*currentTokenHandle)++;
    
    }

    // Doing some post processing after function parsed
    if(!postParsingJobsScope_(scopeBody))
    {
        Log_e(TAG, "Failed to posprocess scope body");
        return ERROR;
    }

    return SUCCESS;
}

static bool postParsingJobsScope_(LocalScopeObjectHandle_t scopeBody)
{
    // Categorizing each bit pack variable to corresponding group
    // Assigning bitpack positions
    // Algorithm of packing should be decided depending on optimization
    if(!Bitfit_assignGroupsAndPositionForVariableHashmap_(&scopeBody->localVariables, FIRST_FIT, &scopeBody->sizeBits))
    {
        Log_e(TAG, "Failed to do bitfitting in scope");
        return ERROR;
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
        (*currentTokenHandle) -= 2;
        
    }else
    {
        Shouter_shoutUnrecognizedToken(cTokenP);
        ParserUtils_skipUntil(currentTokenHandle, SEMICOLON);
    }


    return SUCCESS;
}

static inline bool parseExpressionLine_(LocalScopeObjectHandle_t localScope, TokenHandler_t** currentTokenHandle)
{
    VectorHandler_t expressionVector;
    ExpressionHandle_t stack[1024] = {NULL};

    ExpressionHandle_t lastSymbol = NULL;
    ExpressionHandle_t currentSymbol = NULL;

    int32_t top = -1;

    ALLOC_CHECK(expressionVector, sizeof(Vector_t), ERROR);

    if(!Vector_create(expressionVector, NULL))
    {
        Log_e(TAG, "Failed to create expression vector");
        return NULL;
    }

    while ((cTokenType != BRACKET_END) && (cTokenType != END_FILE) && (cTokenType != SEMICOLON))
    {
        ExpressionHandle_t symbol;

        ALLOC_CHECK(symbol, sizeof(Expression_t), ERROR);
        
        Log_d(TAG, "Parsing symbol in expression line");
        Log_d(TAG, "Symbol vector current size %d available: %d", expressionVector->currentSize, expressionVector->availableSize);
        if(!parseSymbolExpression_(localScope, symbol, currentTokenHandle))
        {
            Shouter_shoutError(cTokenP, "Parse error");
            ParserUtils_skipUntil(currentTokenHandle, SEMICOLON);
            free(symbol);
            return SUCCESS;
        }

        lastSymbol = currentSymbol;
        currentSymbol = symbol;

        if(lastSymbol != NULL)
        {
            Log_d(TAG, "symbols1 check %d", lastSymbol->type);
        }else
        {
            Log_d(TAG, "symbols1 check (null)");
        }

        if(currentSymbol != NULL)
        {
            Log_d(TAG, "symbols2 check %d", currentSymbol->type);
        }else
        {
            Log_d(TAG, "symbols2 check (null)");
        }

        if(!isSymbolsLegalToExistTogether_(lastSymbol, currentSymbol))
        {
            Shouter_shoutError(cTokenP, "Illegal token to use after prev token");
            ParserUtils_skipUntil(currentTokenHandle, SEMICOLON);
            return SUCCESS;
        }

        // If the scanned character is 
        // an operand, add it to the output string.
        if (isSymbolOperand_(symbol))
        {
            if(!Vector_append(expressionVector, symbol))
            {
                return ERROR;
            }
        }

        // If the scanned character is
        // an ‘(‘, push it to the stack.
        else if (symbol->type == EXP_PARENTHESES_LEFT)
        {
            stack[++top] = symbol;
        }

        // If the scanned character is an ‘)’,
        // pop and add to the output string from the stack 
        // until an ‘(‘ is encountered.
        else if (symbol->type == EXP_PARENTHESES_RIGHT) 
        {
            while (top != -1 && stack[top]->type != EXP_PARENTHESES_LEFT) 
            {
                Log_d(TAG, "Symbol vector current size %d available: %d", expressionVector->currentSize, expressionVector->availableSize);
                if(!Vector_append(expressionVector, stack[top--]))
                {
                    return ERROR;
                }
            }
            top--; 
        }

        // If an operator is scanned
        else 
        {
            
            while ((top != -1) && ((expressionPrecedence_(symbol) < expressionPrecedence_(stack[top])) ||
                                 (expressionPrecedence_(symbol) == expressionPrecedence_(stack[top])))) 
            {
                Log_d(TAG, "Symbol vector current size %d available: %d", expressionVector->currentSize, expressionVector->availableSize);
                if(!Vector_append(expressionVector, stack[top--]))
                {
                    return ERROR;
                }
            }

            stack[++top] = symbol;
        }

        (*currentTokenHandle)++;
    }

    currentSymbol = NULL;

    // Pop all the remaining elements from the stack
    while (top != -1) 
    {
        Log_d(TAG, "Symbol vector current size %d available: %d", expressionVector->currentSize, expressionVector->availableSize);
        if(!Vector_append(expressionVector, stack[top--]))
        {
            return ERROR;
        }
    }

    for(int i = 0; i < expressionVector->currentSize; i++)
    {
        ExpressionHandle_t handle =  expressionVector->expandable[i];
        Log_d(TAG, "symbol: %d", handle->type);
    }


    if(!Vector_append(&localScope->expressionList, expressionVector))
    {
        Log_e(TAG, "Failed to append expression to expressionsList vector");
        return ERROR;
    }
    
    Log_d(TAG, "Finished expression line %d %d", tokenOffset(0)->tokenType, tokenOffset(1)->tokenType);
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

    if(!Vector_create(&scopeBody->expressionList, &initialSettingExpressions))
    {
        Log_e(TAG, "Failed to initialize BodyParser expressions vector");
        return ERROR;
    }

    return SUCCESS;
}

static inline bool parseSymbolExpression_(LocalScopeObjectHandle_t scopeBody, ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle)
{
    if(isTokenOperator_(currentTokenHandle))
    {
        Log_d(TAG, "parsing operator: %s", cTokenP->valueString);

        if(!handleOperator_(symbolHandle, currentTokenHandle))
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

                if(!handleNaming_(scopeBody, symbolHandle, currentTokenHandle))
                {
                    Log_e(TAG, "Error happened while handling variable%s in expression", cTokenP->valueString);
                    return ERROR;
                }

            }break;

            case NUMBER_VALUE:
            {
                Log_d(TAG, "parsing number: %s", cTokenP->valueString);

                if(!handleNumber_(symbolHandle, currentTokenHandle))
                {
                    Log_e(TAG, "Error happened while handling number%s in expression", cTokenP->valueString);
                    return ERROR;
                }
            }break;break;

            case BRACKET_ROUND_START: symbolHandle->type = EXP_PARENTHESES_LEFT;break;
            case BRACKET_ROUND_END: symbolHandle->type = EXP_PARENTHESES_RIGHT;break;
            
            default: return ERROR;
        }
    }

    return SUCCESS;

}


static bool handleNumber_(ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle)
{
    AssignValue_t number;
    char *end;
    // in future may need more logic according to this
    symbolHandle->type = EXP_CONST_NUMBER;
    // Parsing number
    number = strtol(cTokenP->valueString, &end, 10);

    symbolHandle->expressionObject = (void*) (uintptr_t) number;
    return SUCCESS;
}

static bool handleOperator_(ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle)
{
    symbolHandle->type = EXP_OPERATOR;

    switch (cTokenType)
    {
        // TODO make work with double operators or even bigger like &&
        case OPERATOR_PLUS: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_PLUS);break;
        case OPERATOR_MINUS: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_MINUS);break;
        case OPERATOR_MODULUS: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_MODULUS);break;
        case OPERATOR_MULTIPLY: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_MULTIPLY);break;
        case OPERATOR_DIVIDE: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_DIVIDE);break;
        case NOT: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_NOT);break;
        case OPERATOR_XOR: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_BIN_XOR);break;
        case OPERATOR_AND: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_BIN_AND);break;
        case OPERATOR_OR: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_BIN_OR);break;
        case OPERATOR_NOT: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_BIN_NOT);break;
        case EQUAL: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_SET);break;

        default:return ERROR;
    }

    return SUCCESS;
}

static bool handleNaming_(LocalScopeObjectHandle_t localScopeBody, ExpressionHandle_t symbol, TokenHandler_t** currentTokenHandle)
{
    // TokenHandler_t currentNamingToken;
    (*currentTokenHandle)++;

    if(cTokenType== BRACKET_ROUND_START)
    {
        ExMethodCallHandle_t methodHandle;

        ALLOC_CHECK(methodHandle, sizeof(ExMethodCall_t), ERROR);

        (*currentTokenHandle)--;
        Log_d(TAG, "Parsing method call: %s", cTokenP->valueString);

        if(!handleMethodCall_(methodHandle, currentTokenHandle, true))
        {
            Log_e(TAG, "Failed to handle method parsing");
            return ERROR;
        }

        symbol->type = EXP_METHOD_CALL;
        symbol->expressionObject = methodHandle;

    }else
    {   
        (*currentTokenHandle)--;
        symbol->type = EXP_VARIABLE;

        const char* varName = cTokenP->valueString;

        Log_d(TAG, "Parsing variable: %s", varName);

        if(Hashmap_find(&localScopeBody->localVariables, varName, strlen(varName)))
        {
            symbol->expressionObject = localScopeBody->localVariables.value;
        }else
        {
            symbol->expressionObject = NULL;
            Shouter_shoutError(cTokenP, "Variable '%s' is not declared previously", varName);
        }

    }
        
    return SUCCESS;
}


static bool handleMethodCall_(ExMethodCallHandle_t methodCall, TokenHandler_t** currentTokenHandle, const bool isMethodSelf)
{
    // TODO: method existance check

    methodCall->method.methodName = cTokenP->valueString;
    methodCall->isMethodSelf = isMethodSelf;
    ALLOC_CHECK(methodCall->method.parameters, sizeof(Vector_t),ERROR);

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
    return false;
}

static bool isTokenOperator_(TokenHandler_t** currentTokenHandle)
{
    return  cTokenType == OPERATOR_PLUS     ||
            cTokenType == OPERATOR_MINUS    ||
            cTokenType == OPERATOR_MODULUS  ||
            cTokenType == OPERATOR_MULTIPLY ||
            cTokenType == OPERATOR_DIVIDE   ||
            cTokenType == OPERATOR_XOR      ||
            cTokenType == OPERATOR_AND      ||
            cTokenType == OPERATOR_OR       ||
            cTokenType == OPERATOR_NOT      ||
            cTokenType == EQUAL;             
            // cTokenType == OPERATOR_DIVIDE;

    // TODO: Add binary operators
}

static bool isSymbolOperand_(const ExpressionHandle_t symbol)
{
    return (symbol->type == EXP_METHOD_CALL) ||
        (symbol->type == EXP_CONST_NUMBER)   ||
        (symbol->type == EXP_VARIABLE);

    // TODO: Add binary operators
}

static bool isSymbolsLegalToExistTogether_(const ExpressionHandle_t firstSymbol, const ExpressionHandle_t secondSymbol)
{
    bool legal = false;

    ExpressionType_t firstType;
    ExpressionType_t secondType;

    if(firstSymbol == NULL)
    {
        firstType = EXP_OPERATOR;
    }else
    {
        firstType = firstSymbol->type;
    }

    if(secondSymbol == NULL)
    {
        secondType = EXP_OPERATOR;
    }else
    {
        secondType = secondSymbol->type;
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
            switch (secondSymbol->type)
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