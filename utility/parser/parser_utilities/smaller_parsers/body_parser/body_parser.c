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
static DynamicStack_t symbolStack;

////////////////////////////////
// PRIVATE METHODS

// static inline bool handleDotAccess_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool handleMethodCall_(LocalScopeObjectHandle_t localScopeBody,
    ExMethodCallHandle_t methodCall,
    TokenHandler_t** currentTokenHandle,
    const VariableObjectHandle_t caller);
static inline bool handleObjectDeclaration_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline bool handleMethodCallParameterization_(LocalScopeObjectHandle_t localScopeBody, VectorHandler_t parameters, TokenHandler_t** currentTokenHandle);
static bool handleOperator_(ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static bool isTokenOperator_(TokenHandler_t token);
static bool handleNaming_(LocalScopeObjectHandle_t localScopeBody, ExpressionHandle_t symbol, TokenHandler_t** currentTokenHandle, const BitpackSize_t castBitSize, const char* castFileType);
// static bool handleOperations_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static bool parseExpressionLine_(LocalScopeObjectHandle_t localScope, VectorHandler_t expressionVector, TokenHandler_t** currentTokenHandle,  TokenHandler_t* expressionEndToken);
static bool isSymbolsLegalToExistTogether_(const ExpressionHandle_t firstSymbol, const ExpressionHandle_t secondSymbol);
static inline bool parseVariableInstance_(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
static inline int32_t expressionPrecedence_(ExpressionHandle_t symbol);
static inline bool parseSymbolExpression_(LocalScopeObjectHandle_t scopeBody, ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static bool handleNumeric_(LocalScopeObjectHandle_t scopeBody, ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle);
static bool handlePostASTMethodCall_(ExMethodCallHandle_t methodCall);
static VariableObjectHandle_t searchVariableNameAcrossScopes(LocalScopeObjectHandle_t localScopeBody, const char* varName);
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
    
    if(!Stack_create(&symbolStack))
    {
        Log_e(TAG, "Couldn't create symbols parsing stack");
        return ERROR;
    }

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
                VectorHandler_t expressionVector = Vector_createDynamic(NULL);

                NULL_GUARD(expressionVector, ERROR, Log_e(TAG, "Failed to create / allocate expression Vector"));

                TokenHandler_t* startExpressionPtr = (*currentTokenHandle);
                ParserUtils_skipUntil(currentTokenHandle, SEMICOLON);
                TokenHandler_t* endExpressionPtr = (*currentTokenHandle);
                
                (*currentTokenHandle) = startExpressionPtr;

                if(!parseExpressionLine_(scopeBody, expressionVector, currentTokenHandle, endExpressionPtr))
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
                    if(!Vector_append(&scopeBody->expressionList, expressionVector))
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

    Stack_destroy(&symbolStack);

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
        ParserUtils_skipUntil(currentTokenHandle, SEMICOLON);
    }


    return SUCCESS;
}

static bool parseExpressionLine_(LocalScopeObjectHandle_t localScope, VectorHandler_t expressionVector, TokenHandler_t** currentTokenHandle,  TokenHandler_t* expressionEndToken)
{
    ExpressionHandle_t lastSymbol = NULL;
    ExpressionHandle_t currentSymbol = NULL;
    int appendsCount = 0;

    while (*currentTokenHandle != expressionEndToken)
    {
        ExpressionHandle_t symbol;

        ALLOC_CHECK(symbol, sizeof(Expression_t), ERROR);
        
        Log_d(TAG, "Parsing symbol in expression line %s", cTokenP->valueString);
        if(!parseSymbolExpression_(localScope, symbol, currentTokenHandle))
        {
            Shouter_shoutError(cTokenP, "Symbol Parse error \'%s\'", cTokenP->valueString);
            ParserUtils_skipUntil(currentTokenHandle, SEMICOLON);
            free(symbol);
            return SUCCESS;
        }
        
        lastSymbol = currentSymbol;
        currentSymbol = symbol;

        if(!isSymbolsLegalToExistTogether_(lastSymbol, currentSymbol))
        {
            Shouter_shoutError(cTokenP, "Illegal token to use after prev token");
            ParserUtils_skipUntil(currentTokenHandle, SEMICOLON);
            return SUCCESS;
        }

        // If the scanned character is 
        // an operand, add it to the output string.
        if (Expression_isSymbolOperand(symbol))
        {
            appendsCount++;

            if(!Vector_append(expressionVector, symbol))
            {
                Log_e(TAG, "Failed to apped operand");
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
            while (!Stack_isEmpty(&symbolStack) && (((ExpressionHandle_t)Stack_peek(&symbolStack))->type != EXP_PARENTHESES_LEFT)) 
            {
                if(!Vector_append(expressionVector, Stack_pop(&symbolStack)))
                {
                    Log_e(TAG, "Failed to append pop from stack");
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
                if(!Vector_append(expressionVector, Stack_pop(&symbolStack)))
                {
                    Log_e(TAG, "Failed to append pop from stack");
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

        if(!Vector_append(expressionVector, Stack_pop(&symbolStack)))
        {
            return ERROR;
        }

    }

    for(int i = 0; i < expressionVector->currentSize; i++)
    {
        ExpressionHandle_t handle =  expressionVector->expandable[i];

        if (handle->type == EXP_CONST_NUMBER)
        {
            Log_i(TAG, "symbol: NUM(%d)", (AssignValue_t) handle->expressionObject);
        }else if(handle->type == EXP_OPERATOR)
        {
            Log_i(TAG, "symbol: OP: %d", (OperatorType_t) handle->expressionObject);
        }else if(handle->type == EXP_VARIABLE)
        {
            VariableObjectHandle_t var = (VariableObjectHandle_t) handle->expressionObject;
            Log_i(TAG, "symbol: bit:%u(%s) %s", var->bitpack, var->castedFile, var->objectName);
        }else if(handle->type == EXP_METHOD_CALL)
        {
            ExMethodCallHandle_t var = (ExMethodCallHandle_t) handle->expressionObject;
            if (var->caller != NULL)
            {
                Log_i(TAG, "symbol: method: %s.%s", var->caller->objectName, var->name);
            }else
            {
                Log_i(TAG, "symbol: method: this.%s", var->name);
            }
            
        }
        
    }
    
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
    if(isTokenOperator_(cTokenP))
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

                if(!handleNaming_(scopeBody, symbolHandle, currentTokenHandle, 0, NULL))
                {
                    Log_e(TAG, "Error happened while handling variable%s in expression", cTokenP->valueString);
                    return ERROR;
                }
             
            }break;

            case NUMBER_VALUE:
            {
                Log_d(TAG, "parsing number: %s", cTokenP->valueString);

                if(!handleNumeric_(scopeBody, symbolHandle, currentTokenHandle))
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

static bool handleNumeric_(LocalScopeObjectHandle_t scopeBody, ExpressionHandle_t symbolHandle, TokenHandler_t** currentTokenHandle)
{
    AssignValue_t number;
    char *end;
    // in future may need more logic according to this
    // symbolHandle->type = EXP_CONST_NUMBER;
    // Parsing number
    number = strtol(cTokenP->valueString, &end, 10);

    symbolHandle->type = EXP_CONST_NUMBER;
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
        case COLON: symbolHandle->expressionObject = (void*) (uintptr_t) (OP_CAST);break;

        default:return ERROR;
    }

    return SUCCESS;
}

static bool handleNaming_(LocalScopeObjectHandle_t localScopeBody, ExpressionHandle_t symbol, TokenHandler_t** currentTokenHandle, const BitpackSize_t castBitSize, const char* castFileType)
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

        symbol->type = EXP_METHOD_CALL;
        symbol->expressionObject = methodHandle;

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

                        symbol->type = EXP_METHOD_CALL;
                        symbol->expressionObject = methodHandle;
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
                symbol->type = EXP_VARIABLE;
                symbol->expressionObject = foundVariableCorresponding;
            }

        }else
        {
            symbol->expressionObject = NULL;
            Shouter_shoutError(cTokenP, "Variable '%s' is not declared previously", varName);
        }
 
    }
        
    return SUCCESS;
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
                if((cTokenType == COMMA) || (*currentTokenHandle) == endExpressionFunctionPtr)
                {
                    endExpressionPtr = (*currentTokenHandle);
                    (*currentTokenHandle) = startExpressionPtr;

                    VectorHandler_t expressionVector = Vector_createDynamic(NULL);

                    NULL_GUARD(expressionVector, ERROR, Log_e(TAG, "Failed to create / allocate expression Vector"));
                    if(!parseExpressionLine_(localScopeBody, expressionVector, currentTokenHandle, endExpressionPtr))
                    {
                        Log_e(TAG, "Failed to parse expression line");
                        return ERROR;
                    }

                    if(!Vector_append(parameters, expressionVector))
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