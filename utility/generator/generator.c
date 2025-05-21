/**
 * @file generator.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-11
 */
#include "generator.h"
#include <vector.h>
#include <global_config.h>
#include <string.h>
#include <logger.h>
#include "../hash/hash.h"
#include <time.h>
#include <queue.h>
#include "arch_specific.h"
#include "../parser/structures/expression/expressions.h"
#include "bit_arithmetic/fit_arithmetic.h"
#include "bit_arithmetic/plt_arithmetic.h"
#include <dstack.h>
#include "first_headers.h"
#include "../parser/parser_utilities/post_parsing_utility/bitfit.h"



////////////////////////////////
// DEFINES
#if ENABLE_READABILITY
    #define READABILITY_ENDLINE             "\n"
    #define READABILITY_SPACE               " "
#else
    #define READABILITY_ENDLINE             ""
    #define READABILITY_SPACE               ""
#endif



#define FWRITE_STRING(string) {if(fwrite(string, BYTE_SIZE, SIZEOF_NOTERM(string), currentCfile_) < 0) {Log_e(TAG, "fwrite failed to write \"%s\"", string);return ERROR;}}

////////////////////////////////
// PRIVATE CONSTANTS

static const char* TAG =                "GENERATOR";

////////////////////////////////
// PRIVATE TYPES
typedef enum
{
    VARIABLE_STRUCT,
    VARIABLE_NORMAL,
    VARIABLE_ASSIGNED,
    VARIABLE_METHOD,
    VARIABLE_PARAMETER
}VariableDeclaration_t;

typedef enum
{
    PUBLIC_CLASS_METHOD,
    PRIVATE_CLASS_METHOD
}NameMangleType_t;

// privateTypes
static MainFrameHandle_t currentAst_ =   NULL;
static FILE* currentCfile_ =             NULL;

static char writingBufferC_[FOUT_BUFFER_LENGTH];

////////////////////////////////
// PRIVATE METHODS

// iterator callbacks
// static int methodBodyVariableInitializerForIterator_(void *key, int count, void* value, void *user);
static int methodDeclarationIteratorCallback_(void *key, int count, void* value, void *user);
static int methodDefinitionIteratorCallback_(void *key, int count, void* value, void *user);

// static bool fileWriteVariableDeclaration_(const VariableObjectHandle_t variable, const VariableDeclaration_t declareType);
static bool fileWriteMethods_();

static bool fileWriteMethodBody_(const MethodObjectHandle_t method);
// static bool handleExpressionWriting_(const ExpressionHandle_t expression);
// static void handleOperatorWritingByType_(const TokenType_t type);
static bool generateMethodHeader_(const MethodObjectHandle_t method);

static inline uint8_t getDigitCountU64_(uint64_t number);
static bool fileWriteNameMangleMethod_(const char* const className, const MethodObjectHandle_t method, const bool isPublic);
static bool fileWriteIncludes_(void);
static bool fileWriteMainHTypedefs_(void);
static bool fileWriteMainHeader_(const bool isFirstFile);
static bool determineResultVariable_(VariableObjectHandle_t resultVariable, const ExpressionHandle_t left, const ExpressionHandle_t right, const OperatorType_t operator);
static bool fileWriteExpression_(const VectorHandler_t expression, VariableObjectHandle_t resultVar, const char* tmpSuffix);
static inline bool fileWriteVariablesAllocation_(const BitpackSize_t bitsize, const uint32_t scopeIndex);
static bool printBitVariableReading_(const ExpressionHandle_t operand);
static bool generateCodeForOperation_(const VariableObjectHandle_t assignedTmpVar, ExpressionHandle_t left, ExpressionHandle_t right, const OperatorType_t operator);
static bool fileWriteBitVariableSet_(const VariableObjectHandle_t assignedTmpVar, const ExpressionHandle_t left, const ExpressionHandle_t right);
static bool getResultingVariableFromOperand_(const ExpressionHandle_t symbol, VariableObjectHandle_t resultVariable);
static bool handleCastOperator_(const VariableObjectHandle_t assignedTmpVar, const ExpressionHandle_t left, const ExpressionHandle_t right);
static bool generateMethodCallScope_(const BitpackSize_t returnSizeBits, const VariableObjectHandle_t assignedTmpVar, const uint64_t funcId, ExMethodCallHandle_t functionExpression);
static bool fileWriteNameMangleParams_(const VectorHandler_t params);
static bool generateCodeForOneOperand_(const ExpressionHandle_t symbol, VariableObjectHandle_t resultVariable);

static inline uint8_t getBitCountU64_(uint64_t number);


////////////////////////////////
// IMPLEMENTATION

bool Generator_generateCode(const MainFrameHandle_t ast, const char* dstCFileName, const bool isFirstFile)
{
    
    NULL_GUARD(ast, ERROR, Log_e(TAG, "Null AST"));

    currentAst_ = ast;
    
    Log_i(TAG, "Starting C code generation in file: \"%s\"", dstCFileName);

    if(dstCFileName == NULL)
    {
        Log_e(TAG, "Failed to allocate memory file paths for ig: %s", dstCFileName);
        return ERROR;
    }

    currentCfile_ = fopen(dstCFileName, "w");

    NULL_GUARD(currentCfile_, ERROR, Log_e(TAG, "Failed to open %s", dstCFileName));

    setvbuf(currentCfile_, writingBufferC_, _IOFBF, FOUT_BUFFER_LENGTH);

    
    // // Generating public methods
    if(!fileWriteMainHeader_(isFirstFile))
    {
        Log_e(TAG, "Failed to write main header");
        return ERROR;
    }

    if(!fileWriteMethods_())
    {
        Log_e(TAG, "Failed to write c class methods");
        return ERROR;
    }

    if(fflush(currentCfile_))  // flushing what is left in buffer
    {
        Log_e(TAG, "Failed to flush file %s buffer", currentCfile_);
        return ERROR;
    }

    if(fclose(currentCfile_))
    {
        Log_w(TAG, "Failed to close file %s", currentCfile_);
    }

    Log_i(TAG, "C code generation in file: \"%s\" SUCCESSFUL!", dstCFileName);


    return SUCCESS;
}



static bool fileWriteMainHeader_(const bool isFirstFile)
{
    int status;
    // first file needs some more stuff to have
    if(isFirstFile)
    {
        status = fwrite(START_POINT, BYTE_SIZE, SIZEOF_NOTERM(START_POINT), currentCfile_);
        if(status < 0)
        {
            Log_e(TAG, "Failed to write starting point injection");
            return ERROR;
        }
    }

    if(!fileWriteIncludes_())
    {
        Log_e(TAG, "Failed to write main #includes in object:%s", currentAst_->iguanaObjectName);
        return ERROR;
    }

    if(!fileWriteMainHTypedefs_())
    {
        Log_e(TAG, "Failed to write main type definitions in object:%s", currentAst_->iguanaObjectName);
        return ERROR;
    }

    return SUCCESS;
}

static bool fileWriteIncludes_(void)
{
    FWRITE_STRING(INCLUDE_WRAP("stdint"));
    FWRITE_STRING(INCLUDE_WRAP("stdio") READABILITY_ENDLINE);

    return SUCCESS;
}

static bool fileWriteMainHTypedefs_(void)
{
    const int writeStatus = fwrite(BITPACK_TYPE_TYPEDEF_DEF READABILITY_ENDLINE, BYTE_SIZE, SIZEOF_NOTERM(BITPACK_TYPE_TYPEDEF_DEF READABILITY_ENDLINE), currentCfile_);
    return (writeStatus >= 0);
}

static bool fileWriteNameMangleParams_(const VectorHandler_t params)
{
    int writeStatus;

    if(params->currentSize > 0)
    {
        for(uint32_t parameterIdx = 0; parameterIdx < params->currentSize; parameterIdx++)
        {
            const VariableObjectHandle_t varParam = (VariableObjectHandle_t) params->expandable[parameterIdx];
            writeStatus = fprintf(currentCfile_, "%u" BIT_DEF "%lu", (uint8_t) SIZEOF_NOTERM(BIT_DEF) + getDigitCountU64_(varParam->bitpack), varParam->bitpack);
            
            if(writeStatus < 0)
            {
                return ERROR;
            }
        }

        writeStatus = fwrite(ASM_FOOTER_MANGLE SEMICOLON_DEF, BYTE_SIZE, SIZEOF_NOTERM(ASM_FOOTER_MANGLE SEMICOLON_DEF), currentCfile_);

        if(writeStatus < 0)
        {
            return ERROR;
        }

    }else
    {
        writeStatus = fwrite(MANGLE_TYPE_VOID_DEF ASM_FOOTER_MANGLE SEMICOLON_DEF, BYTE_SIZE, SIZEOF_NOTERM(MANGLE_TYPE_VOID_DEF ASM_FOOTER_MANGLE SEMICOLON_DEF), currentCfile_);

        if(writeStatus < 0)
        {
            return ERROR;
        }
    }
    
    if(writeStatus < 0)
    {
        return ERROR;
    }

    return SUCCESS;
}

static bool fileWriteNameMangleMethod_(const char* const className, const MethodObjectHandle_t method, const bool isPublic)
{
    int writeStatus;
    // TODO: optimize this so length will be somewhere stored entire generator
    size_t objectNameLen = strlen(className);
    size_t methodNameLen = strlen(method->methodName);

    writeStatus = fprintf(currentCfile_,
        //ex: asm("_ZN9wikipedia3fooEv");
        READABILITY_SPACE ASM_HEADER_MANGLE MANGLE_MAGIC_BYTE_DEF MANGLE_NEST_ID_DEF "%lu" BIT_DEF "%lu_%s%ld%s" MANGLE_END_DEF,
        objectNameLen + ((uint8_t) SIZEOF_NOTERM(BIT_DEF)) + ((uint8_t) SIZEOF_NOTERM("_")) + getDigitCountU64_((uint64_t) currentAst_->objectSizeBits),
        currentAst_->objectSizeBits,
        className,
        methodNameLen,
        method->methodName);

    if(writeStatus < 0)
    {
        return ERROR;
    }

    if(!fileWriteNameMangleParams_(method->parameters))
    {
        Log_e(TAG, "Failed to write params mangle of method");
        return ERROR;
    }
    
    return SUCCESS;
}


static inline bool fileWriteMethods_()
{
    // generating function definitions
    Hashmap_forEach(&currentAst_->methods, methodDefinitionIteratorCallback_, NULL);

    // Generating function declarations
    Hashmap_forEach(&currentAst_->methods, methodDeclarationIteratorCallback_, NULL);

    return SUCCESS;
}

static inline bool fileWriteVariablesAllocation_(const BitpackSize_t bitsize, const uint32_t scopeId)
{
    const int status = fprintf(currentCfile_, BITPACK_TYPE_NAME " " STRINGIFY(ALLOCATION_ARRAY_PREFIX)"%u[%lu]" SEMICOLON_DEF READABILITY_ENDLINE, scopeId, bitsize / BIT_SIZE_BITPACK + 1);
    return (status >= 0);
}

static inline bool fileWriteMethodBody_(const MethodObjectHandle_t method)
{

    VariableObject_t resultVar;

    resultVar.objectName = NULL;

    fwrite(READABILITY_ENDLINE BRACKET_START_DEF READABILITY_ENDLINE, BYTE_SIZE, SIZEOF_NOTERM(READABILITY_ENDLINE BRACKET_START_DEF READABILITY_ENDLINE), currentCfile_);

     if(!fileWriteVariablesAllocation_(method->body.sizeBits, 0))
    {
        Log_e(TAG, "Failed to write method scope variables");
        return ERROR;
    }
    
    for(size_t expressionQ = 0; expressionQ < method->body.expressionList.currentSize; expressionQ++)
    {
        VectorHandler_t expression;

        expression = method->body.expressionList.expandable[expressionQ];
        
        if(expression == NULL)
        {
            return ERROR;
        }


        if(!fileWriteExpression_(expression, &resultVar, "_"))
        {
            Log_e(TAG, "Failed to write expression");
            return ERROR;
        }
        
        
    }

    FWRITE_STRING(READABILITY_ENDLINE BRACKET_END_DEF READABILITY_ENDLINE);
    
    return SUCCESS;
}


static bool fileWriteExpression_(const VectorHandler_t expression, VariableObjectHandle_t resultVar, const char* tmpSuffix)
{
    DynamicStack_t symbolStack;
    uint64_t tmpIncrement = 0;

    ExpressionHandle_t tmpExpression;
    VariableObjectHandle_t tmpVar;
    char* currSufix;

    if(!Stack_create(&symbolStack))
    {
        Log_e(TAG, "Failed to create dynamic stack for postfix handling");
        return ERROR;
    }

    if(resultVar->objectName != NULL)
    {
        fprintf(currentCfile_, BIT_TYPE_DEF READABILITY_SPACE "%s" SEMICOLON_DEF READABILITY_ENDLINE, resultVar->objectName);
    }

    FWRITE_STRING(BRACKET_START_DEF READABILITY_ENDLINE);

    // there is one element pushed in postfix
    // May be a operand just laying around
    if(expression->currentSize == 1)
    {
        const ExpressionHandle_t symbol = (ExpressionHandle_t) expression->expandable[0];

        // If operand handle it, if operator or something else ignore, it shouldn't safely passby from parser side
        if(Expression_isSymbolOperand(symbol))
        {
            if(!generateCodeForOneOperand_(symbol, resultVar))
            {
                Log_e(TAG, "Failed to generate code for 1 operand");
                return ERROR;
            }
        }
        
    }else
    {
        for(size_t symbolIdx = 0; symbolIdx < expression->currentSize; symbolIdx++)
        {
            const ExpressionHandle_t symbol = (ExpressionHandle_t) expression->expandable[symbolIdx];

            if(Expression_isSymbolOperand(symbol))
            {
                Stack_push(&symbolStack, symbol);
            }else if(Expression_isSymbolOperator(symbol))
            {

                // Generating temp variable to store for this operation
                tmpExpression = alloca(sizeof(Expression_t));
                NULL_GUARD(tmpExpression, ERROR, Log_e(TAG, "Failed to allocate tmpExpression"));
                
                tmpVar = alloca(sizeof(VariableObject_t));
                NULL_GUARD(tmpVar, ERROR, Log_e(TAG, "Failed to allocate tmpVar"));

                currSufix = alloca(strlen(tmpSuffix) + 10);
                NULL_GUARD(currSufix, ERROR, Log_e(TAG, "Failed to allocate currSfix"));

                currSufix[0] = '\0'; 
                
                const ExpressionHandle_t right = Stack_pop(&symbolStack);
                const ExpressionHandle_t left = Stack_pop(&symbolStack);
                
                if(right == NULL || left == NULL)
                {
                    Log_w(TAG, "stack got popped null values");
                    break;
                }

                const OperatorType_t operator = (OperatorType_t) symbol->expressionObject;

                sprintf(currSufix, "%s" STRINGIFY(TMP_VAR) "%lu", tmpSuffix, tmpIncrement);
                
                if(!determineResultVariable_(tmpVar, left, right, operator))
                {
                    Log_e(TAG, "error in determining result variable");
                    return ERROR;
                }
                
                tmpVar->objectName = currSufix;

                tmpExpression->type = EXP_TMP_VAR;
                tmpExpression->expressionObject = tmpVar;
                
                // DO STUFF
                if(!generateCodeForOperation_(tmpVar, left, right, operator))
                {
                    Log_e(TAG, "Failed to generate code for operation");
                    return ERROR;
                }

                if(!Stack_push(&symbolStack, tmpExpression))
                {
                    Log_e(TAG, "Failed to push stack expression");
                    return ERROR;
                }

                tmpIncrement++;
            }
        }
        resultVar->bitpack = tmpVar->bitpack;
        resultVar->posBit = tmpVar->posBit;
        resultVar->belongToGroup = tmpVar->belongToGroup;
        resultVar->castedFile = tmpVar->castedFile;
        // Var name left, since it passed through object, through params
        
        if(resultVar->objectName != NULL)
        {
            fprintf(currentCfile_, "%s" READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE "%s" SEMICOLON_DEF READABILITY_ENDLINE, resultVar->objectName, tmpVar->objectName);
        }
    }



    FWRITE_STRING(BRACKET_END_DEF READABILITY_ENDLINE);

    Stack_destroy(&symbolStack);
    
    return SUCCESS;
}

static bool determineResultVariable_(VariableObjectHandle_t resultVariable, const ExpressionHandle_t left, const ExpressionHandle_t right, const OperatorType_t operator)
{
    return SUCCESS;
}


static bool getResultingVariableFromOperand_(const ExpressionHandle_t symbol, VariableObjectHandle_t resultVariable)
{
    NULL_GUARD(resultVariable, ERROR, Log_e(TAG, "Passed NULL result variable for bit count estimation"));

    switch (symbol->type)
    {
        case EXP_TMP_VAR:
        {
            VariableObjectHandle_t var = (VariableObjectHandle_t) symbol->expressionObject;
            NULL_GUARD(var, ERROR, Log_e(TAG, "NULL temp variable detected for bit count estimation"));

            resultVariable->bitpack = var->bitpack;
        }break;

        case EXP_VARIABLE:
        {
            VariableObjectHandle_t var = (VariableObjectHandle_t) symbol->expressionObject;
            NULL_GUARD(var, ERROR, Log_e(TAG, "NULL variable detected for bit count estimation"));
            resultVariable->bitpack = var->bitpack;
        }break;

        case EXP_CONST_NUMBER:
        {
            AssignValue_t value = (AssignValue_t) symbol->expressionObject;
            resultVariable->bitpack = getBitCountU64_(value);
            
        }break;
        
        case EXP_METHOD_CALL:
        {
            // if its just method call then return type is 0, cast case makes tmp var non zero
            resultVariable->bitpack = 0;
        }break;

        default:
        {
            Log_e(TAG, "Unhandled operator in 1 operator parse type: %d", symbol->type);
        }return ERROR;
    }

    return SUCCESS;
}

static bool generateCodeForOneOperand_(const ExpressionHandle_t symbol, VariableObjectHandle_t resultVariable)
{
    if(!getResultingVariableFromOperand_(symbol, resultVariable))
    {
        Log_e(TAG, "Failed to estimate bit count for symbol");
        return ERROR;
    }
    if(resultVariable->objectName != NULL)
    {
        fprintf(currentCfile_, BIT_TYPE_DEF " %s" C_OPERATOR_EQUAL_DEF READABILITY_SPACE, resultVariable->objectName);
    }

    if(symbol->type != EXP_METHOD_CALL)
    {
        if(!printBitVariableReading_(symbol))
        {
            return ERROR;
        }

        FWRITE_STRING(SEMICOLON_DEF READABILITY_ENDLINE);
    }else
    {
        ExMethodCallHandle_t methodCall = symbol->expressionObject;

        if(!generateMethodCallScope_(0, resultVariable, 0, methodCall))
        {
            Log_e(TAG, "Failed to generate method call for one operand");
            return ERROR;
        }
    }    

    return SUCCESS;
}

static bool generateMethodCallScope_(const BitpackSize_t returnSizeBits, const VariableObjectHandle_t assignedTmpVar, const uint64_t funcId, ExMethodCallHandle_t method)
{
    Vector_t resultVars;


    char* currSufix;
    if(assignedTmpVar->objectName == NULL)
    {
        currSufix = alloca(10);
    }else
    {
        currSufix = alloca(strlen(assignedTmpVar->objectName) + 10);
    }

    NULL_GUARD(currSufix, ERROR, Log_e(TAG, "Failed to allocate currSfix"));

    currSufix[0] = '\0';
    if(assignedTmpVar->objectName == NULL)
    {
        sprintf(currSufix, "f%lu", funcId);
    }else
    {
        sprintf(currSufix, "%sf%lu", assignedTmpVar->objectName, funcId);
    }
    
    fprintf(currentCfile_, BIT_TYPE_DEF " %s" SEMICOLON_DEF READABILITY_ENDLINE BRACKET_START_DEF READABILITY_ENDLINE, currSufix);

    Log_d(TAG, "Start on method call generation: %s", method->name);

    if(!Vector_create(&resultVars, NULL))
    {
        Log_e(TAG, "Failed to create result vars vector");
        return ERROR;
    }

    for(size_t paramIdx = 0; paramIdx < method->parameters.currentSize; paramIdx++)
    {
        VariableObjectHandle_t resultVar = alloca(sizeof(VariableObject_t));

        char* paramName = alloca(strlen(assignedTmpVar->objectName) + 10);
        NULL_GUARD(paramName, ERROR, Log_e(TAG, "Failed to allocate param name"));
        paramName[0] = '\0'; 

        sprintf(paramName, "p%lu", paramIdx);
        resultVar->objectName = paramName;

        if(!fileWriteExpression_(method->parameters.expandable[paramIdx], resultVar, currSufix))
        {
            Log_e(TAG, "Failed to write parameter expression");
            return ERROR;
        }

        if(!Vector_append(&resultVars, resultVar))
        {
            Log_e(TAG, "Failed to append to result variables");
            return ERROR;
        }

    }

    // void case not needed to pack
    if (returnSizeBits != 0)
    {
        VariableObject_t returnVar;

        returnVar.bitpack = returnSizeBits;

        // Adding return variable also to bitfit
        if(!Vector_append(&resultVars, &returnVar))
        {
            Log_e(TAG, "Failed to append to result variables");
            return ERROR;
        }
    }
    
    BitpackSize_t sizeNeededForFunctionParams;

    if(!Bitfit_assignGroupsAndPositionForVariableVector_(&resultVars, FIRST_FIT, &sizeNeededForFunctionParams))
    {
        Log_e(TAG, "Failed to fit params bits");
        return ERROR;
    }

    if(resultVars.currentSize > 0)
    {
        fprintf(currentCfile_, EXTERN_KEYWORD_DEF " " TYPE_BIT0_DEF " " "%s" BRACKET_ROUND_START_DEF "" READABILITY_SPACE "void* p" BRACKET_ROUND_END_DEF, method->name);
    }else
    {
        fprintf(currentCfile_, EXTERN_KEYWORD_DEF " " TYPE_BIT0_DEF " " "%s" BRACKET_ROUND_START_DEF TYPE_BIT0_DEF BRACKET_ROUND_END_DEF, method->name);
    }

    MethodObject_t tempMethodObj;

    tempMethodObj.methodName = method->name;
    tempMethodObj.parameters = &resultVars;
    
    // if(method->caller == NULL)
    // {
    //     VariableObjectHandle_t caller = malloc(sizeof(VariableObject_t));
    //     caller->objectName = currentAst_->iguanaObjectName;

    //     method->caller = caller;
    // }
    if(method->caller == NULL)
    {
        if(!fileWriteNameMangleMethod_(currentAst_->iguanaObjectName, &tempMethodObj, true))
        {
            Log_e(TAG, "Failed to write mangle self method \'%s\'", method->name);
            return ERROR;
        }

    }else
    {
        if(!fileWriteNameMangleMethod_(method->caller->objectName, &tempMethodObj, true))
        {
            Log_e(TAG, "Failed to write mangle method \'%s\'", method->name);
            return ERROR;
        }

    }

    if(resultVars.currentSize > 0)
    {
        fprintf(currentCfile_, READABILITY_ENDLINE "%s" BRACKET_ROUND_START_DEF "" READABILITY_SPACE "%spset" BRACKET_ROUND_END_DEF SEMICOLON_DEF READABILITY_ENDLINE, method->name, assignedTmpVar->objectName);
    }else
    {
        fprintf(currentCfile_, READABILITY_ENDLINE "%s" BRACKET_ROUND_START_DEF BRACKET_ROUND_END_DEF SEMICOLON_DEF READABILITY_ENDLINE, method->name);
    }


    FWRITE_STRING(BRACKET_END_DEF READABILITY_ENDLINE);

    // const ExMethodCallHandle_t call = operand->expressionObject;

    // if(!filewriteMethodCall_(call))
    // {
    //     Log_e(TAG, "Failed to write method call");
    //     return ERROR;   
    // }

    // functionExpression->expressionObject = assignedTmpForOperation;
    return SUCCESS;
}

static bool generateCodeForOperation_(const VariableObjectHandle_t assignedTmpVar, ExpressionHandle_t left, ExpressionHandle_t right, const OperatorType_t operator)
{
    char* operatorString = NULL;
    int status;

    // Checking if operation regenerateCodeForOperation_lated to function call, it needs be handled differently
    if(left->type == EXP_METHOD_CALL)
    {
        
        if(!generateMethodCallScope_(0, assignedTmpVar, 0, left->expressionObject))
        {
            Log_e(TAG, "Failed to generate method call scope 0 (left)");
            return ERROR;
        }
    }

    if(right->type == EXP_METHOD_CALL)
    {
        if(!generateMethodCallScope_(0, assignedTmpVar, 1, right->expressionObject))
        {
            Log_e(TAG, "Failed to generate method call scope 1 (right)");
            return ERROR;
        }
    }

    if(operator == OP_CAST)
    {
        return handleCastOperator_(assignedTmpVar, left, right);
    }


    // Set handling differently
    if(operator != OP_SET)
    {
        fprintf(currentCfile_, BIT_TYPE_DEF " %s" READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE, assignedTmpVar->objectName);

        if(!printBitVariableReading_(left))
        {
            return ERROR;
        }
    }else
    {
        return fileWriteBitVariableSet_(assignedTmpVar, left, right);
    }

    switch (operator)
    {
        case OP_PLUS: operatorString =          READABILITY_SPACE C_OPERATOR_PLUS_DEF READABILITY_SPACE; break;
        case OP_MINUS: operatorString =         READABILITY_SPACE C_OPERATOR_MINUS_DEF READABILITY_SPACE; break;
        case OP_MULTIPLY: operatorString =      READABILITY_SPACE C_OPERATOR_MULTIPLY_DEF READABILITY_SPACE; break;
        case OP_DIVIDE: operatorString =        READABILITY_SPACE C_OPERATOR_DIVIDE_DEF READABILITY_SPACE; break;
        case OP_MODULUS: operatorString =       READABILITY_SPACE C_OPERATOR_MODULUS_DEF READABILITY_SPACE; break;
        case OP_SET: operatorString =           READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE; break;
        case OP_BIN_AND: operatorString =       READABILITY_SPACE C_OPERATOR_BIN_AND_DEF READABILITY_SPACE; break;
        // case OP_BIN_NOT: operatorString =    READABILITY_SPACE C_OPERATOR_DIVIDE_DEF READABILITY_SPACE; break;
        case OP_BIN_OR: operatorString =        READABILITY_SPACE C_OPERATOR_BIN_OR_DEF READABILITY_SPACE; break;
        case OP_BIN_XOR: operatorString =       READABILITY_SPACE C_OPERATOR_BIN_XOR_DEF READABILITY_SPACE; break;

        default: operatorString = NULL; break;
    }

    status = fwrite(operatorString, BYTE_SIZE, strlen(operatorString), currentCfile_);

    if(status < 0)
    {
        Log_e(TAG, "Failed to write operator string %s", operatorString);
        return ERROR;
    }

    if(!printBitVariableReading_(right))
    {
        return ERROR;
    }

    FWRITE_STRING(SEMICOLON_DEF READABILITY_ENDLINE)

    return SUCCESS;
}

static bool printBitVariableReading_(const ExpressionHandle_t operand)
{
    int status = SUCCESS;


    if (operand->type == EXP_VARIABLE)
    {
        const VariableObjectHandle_t variable = (VariableObjectHandle_t) operand->expressionObject;

        Log_d(TAG, "Variable name: %s variable.pos=%u variable_bitpack:%lu", variable->objectName, variable->posBit, variable->bitpack);

        if(variable->bitpack < BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY((AFIT_READ(s_%u[%u], %u, %lu)&MASK(%lu))), 0, variable->belongToGroup, variable->posBit, variable->bitpack, variable->bitpack);
        }else if (variable->bitpack == BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(APLT_READ(s_%u[%u])), 0, variable->belongToGroup);
        }
        printf("Tf\n");
    }else if(operand->type == EXP_TMP_VAR)
    {
        VariableObjectHandle_t var = (VariableObjectHandle_t) operand->expressionObject;

        status = fprintf(currentCfile_, STRINGIFY(APLT_READ(%s)), var->objectName);
    }else if(operand->type == EXP_CONST_NUMBER)
    {
        status = fprintf(currentCfile_, STRINGIFY(APLT_READ(%lu)), (AssignValue_t) operand->expressionObject);
    }

    return status > 0;
}


static bool fileWriteBitVariableSet_(const VariableObjectHandle_t assignedTmpVar, const ExpressionHandle_t left, const ExpressionHandle_t right)
{
    int status;
    const VariableObjectHandle_t leftVar = (VariableObjectHandle_t) left->expressionObject;
    const VariableObjectHandle_t rightVar = (VariableObjectHandle_t) right->expressionObject;
    
    if(left->type == EXP_VARIABLE)
    {
        if(leftVar->bitpack < BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_,STRINGIFY(s_%u[%u] = AFIT_RESET(s_%u[%u], %u, %lu)) READABILITY_SPACE C_OPERATOR_BIN_OR_DEF READABILITY_SPACE,
                 0,
                leftVar->belongToGroup, 0,
                leftVar->belongToGroup,
                leftVar->bitpack, leftVar->posBit, leftVar->bitpack);

        }else if (leftVar->bitpack == BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(s_%u[%u]) READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE, 
            0, leftVar->belongToGroup);
        }else
        {
            Log_e(TAG, "Unhandled case vars cant be now bigger than %u", BIT_SIZE_BITPACK);
            return ERROR;
        }
    }else if(left->type == EXP_CONST_NUMBER)
    {
        Log_e(TAG, "Unhandled case %ld = value", (AssignValue_t) left->expressionObject);
        return ERROR;
    }
    
    if(right->type == EXP_TMP_VAR)
    {
        VariableObjectHandle_t var = (VariableObjectHandle_t) right->expressionObject;
        status = fprintf(currentCfile_, STRINGIFY(((%s) << (BIT_SIZE_BITPACK - (%u + %lu)))) SEMICOLON_DEF READABILITY_ENDLINE, var->objectName, leftVar->posBit, leftVar->bitpack);
        status = fprintf(currentCfile_, BIT_TYPE_DEF READABILITY_SPACE "%s" READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE "%s" SEMICOLON_DEF READABILITY_ENDLINE, assignedTmpVar->objectName, var->objectName);

    }else if (right->type == EXP_VARIABLE)
    {
        if(rightVar->bitpack < BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(((AFIT_READ(s_%u[%u], %u, %lu) & MASK(%lu)) << (BIT_SIZE_BITPACK - (%u + %lu)))) SEMICOLON_DEF READABILITY_ENDLINE, 0, rightVar->belongToGroup, rightVar->posBit, rightVar->bitpack, leftVar->bitpack, leftVar->posBit, leftVar->bitpack);
            status = fprintf(currentCfile_, BIT_TYPE_DEF READABILITY_SPACE "%s" READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE STRINGIFY(AFIT_READ(s_%u[%u], %u, %lu) & MASK(%lu)) SEMICOLON_DEF READABILITY_ENDLINE, assignedTmpVar->objectName, 0, rightVar->belongToGroup, rightVar->posBit, rightVar->bitpack, leftVar->bitpack);
        }else if (rightVar->bitpack == BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(((s_%u[%u] & MASK(%lu)) << (BIT_SIZE_BITPACK - (%u + %lu)))) SEMICOLON_DEF READABILITY_ENDLINE, 0, rightVar->belongToGroup, leftVar->bitpack, leftVar->posBit, leftVar->bitpack);
            status = fprintf(currentCfile_, BIT_TYPE_DEF READABILITY_SPACE "%s" READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE STRINGIFY(s_%u[%u] & MASK(%lu)) SEMICOLON_DEF READABILITY_ENDLINE, assignedTmpVar->objectName, 0, rightVar->belongToGroup, leftVar->bitpack);
        }else
        {   
            Log_e(TAG, "Unhandled case vars cant be now bigger than %u", BIT_SIZE_BITPACK);
            return ERROR;
        }
    }else if(right->type == EXP_CONST_NUMBER)
    {
        const AssignValue_t constValue = (AssignValue_t)right->expressionObject;

        status = fprintf(currentCfile_, STRINGIFY(((%ld & MASK(%lu)) << (BIT_SIZE_BITPACK - (%u + %lu)))) SEMICOLON_DEF READABILITY_ENDLINE, constValue, leftVar->bitpack, leftVar->posBit, leftVar->bitpack);
        status = fprintf(currentCfile_, BIT_TYPE_DEF READABILITY_SPACE "%s" READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE "%lu" SEMICOLON_DEF READABILITY_ENDLINE, assignedTmpVar->objectName, constValue);
    }
   
    return (status > 0);
}


static int methodDefinitionIteratorCallback_(void *key, int count, void* value, void *user)
{
    MethodObjectHandle_t method;
    method = value;
    if(!generateMethodHeader_(method))
    {
        Log_e(TAG, "Failed to write method %s header", method->methodName);
        return ERROR;
    }

    if(!fileWriteNameMangleMethod_(currentAst_->iguanaObjectName, method, true))
    {
        Log_e(TAG, "Failed to write name mangling for method %s", method->methodName);
        return ERROR;
    }

    fwrite(END_LINE_DEF, BYTE_SIZE, 1, currentCfile_);    

    return SUCCESS;
}


static bool generateMethodHeader_(const MethodObjectHandle_t method)
{
    
    if(method->containsBody)
    {
        fprintf(currentCfile_, "void %s" BRACKET_ROUND_START_DEF, method->returnVariable->objectName);
    }

    if(Hashmap_size(&currentAst_->classVariables) != 0)
    {
        if(method->containsBody)
        {
            // TODO later change this hardcoded 
            // fwrite(PARAM_TYPE_DEF READABILITY_SPACE FUNCTION_OBJ_NAME COMMA_DEF READABILITY_SPACE,
            //     BYTE_SIZE,
            //     SIZEOF_NOTERM(PARAM_TYPE_DEF READABILITY_SPACE FUNCTION_OBJ_NAME COMMA_DEF READABILITY_SPACE), currentCfile_);
            
            fwrite(READABILITY_SPACE,
                BYTE_SIZE,
                SIZEOF_NOTERM(READABILITY_SPACE), currentCfile_);

        }
    }

    if(method->parameters->currentSize > 0)
    {
        FWRITE_STRING(PARAM_TYPE_DEF READABILITY_SPACE FUNCTION_PARAM_NAME);
    }else
    {
        FWRITE_STRING(TYPE_BIT0_DEF);
    }
    
    FWRITE_STRING(BRACKET_ROUND_END_DEF);

    return SUCCESS;
}

static int methodDeclarationIteratorCallback_(void *key, int count, void* value, void *user)
{
    MethodObjectHandle_t method;
    method = value;

    NULL_GUARD(method, ERROR, Log_e(TAG, "AST method '%s' is NULL", key));

    if(method->accessType == IGNORED)
    {
        // If annotated as ignored, lets call it as generated without generation
        return SUCCESS;
    }
    
    if(!generateMethodHeader_(method))
    {
        Log_e(TAG, "Failed to generate method %s header", method->methodName);
        return ERROR;
    }

    if(method->containsBody)
    {
        if(!fileWriteMethodBody_(method))
        {
            Log_e(TAG, "Failed to write method body to IO");
            return ERROR;
        }
    }else
    {
        // TODO abstracting fwrites
        fwrite(SEMICOLON_DEF, BYTE_SIZE, 1, currentCfile_);
    }
    return SUCCESS;
}


// static bool filewriteMethodCall_(const BitpackSize_t returnSizeBits, const ExMethodCallHandle_t methodCallHandle)
// {

//     // //TODO: when implemented function normal calls will be, will add better handling, for now lets leave
//     // if(strcmp(methodCallHandle->name, "print") == 0)
//     // {
//     //     FWRITE_STRING("printf(\"");
//     //     for(uint16_t paramIdx = 0; paramIdx < methodCallHandle->parameters.currentSize; paramIdx++)
//     //     {
//     //         FWRITE_STRING("%lu ");
//     //     }
//     //     FWRITE_STRING("\\n\"");

//     //     for(uint16_t paramIdx = 0; paramIdx < methodCallHandle->parameters.currentSize; paramIdx++)
//     //     {
//     //         FWRITE_STRING("," READABILITY_SPACE);

//     //         const ExpressionHandle_t param = methodCallHandle->parameters.expandable[paramIdx];

//     //         if(!printBitVariableReading_(param))
//     //         {
//     //             Log_e(TAG, "Failed to generate param for print");
//     //             return ERROR;
//     //         }
        
//     //     }
//     //     FWRITE_STRING(BRACKET_ROUND_END_DEF);
//     // }else
//     // {
        
//     //     // printf("%s from caller: %s %lu", methodCallHandle->name, methodCallHandle->castFile, methodCallHandle->castBitSize);

//     //     FWRITE_STRING("0");
//     // }

//     return SUCCESS;
// }


static bool handleCastOperator_(const VariableObjectHandle_t assignedTmpVar, const ExpressionHandle_t left, const ExpressionHandle_t right)
{
    if(left->type != EXP_CONST_NUMBER)
    {
        Log_e(TAG, "For now non constant (dynamic) casting is not allowed, will be in future");
        return ERROR;
    }

    if(right->type == EXP_METHOD_CALL)
    {
        AssignValue_t castValue = (AssignValue_t) left->expressionObject;

        if(!generateMethodCallScope_(castValue, assignedTmpVar, 0, right->expressionObject))
        {
            Log_e(TAG, "Failed to write method call to file size: %lu", castValue);
            return ERROR;
        }

    }else if(right->type == EXP_CONST_NUMBER)
    {
        Log_e(TAG, "Cast on constant value not supported yet");
        return ERROR;
    }else if(right->type == EXP_TMP_VAR)
    {
        Log_e(TAG, "Casting on TMP var not supported yet");
        return ERROR;
    }else if(right->type == EXP_VARIABLE)
    {
        VariableObjectHandle_t var = (VariableObjectHandle_t) right->expressionObject;
        Log_e(TAG, "Casting on variable not supported yet \'%s\'", var->objectName);
        return ERROR;
    }else
    {
        Log_e(TAG, "Unknown casting type found");
        return ERROR;
    }

    return SUCCESS;
}

static inline uint8_t getBitCountU64_(uint64_t number)
{
    uint8_t count = 0;
    while (number) {
        count++;
        number >>= 1;
    }
    return count;
}

static inline uint8_t getDigitCountU64_(uint64_t number) 
{
    if (number < 10ULL) {return 1;}
    if (number < 100ULL) {return 2;}
    if (number < 1000ULL) {return 3;}
    if (number < 10000ULL) {return 4;}
    if (number < 100000ULL) {return 5;}
    if (number < 1000000ULL) {return 6;}
    if (number < 10000000ULL) {return 7;}
    if (number < 100000000ULL) {return 8;}
    if (number < 1000000000ULL) {return 9;}
    if (number < 10000000000ULL) {return 10;}
    if (number < 100000000000ULL) {return 11;}
    if (number < 1000000000000ULL) {return 12;}
    if (number < 10000000000000ULL) {return 13;}
    if (number < 100000000000000ULL) {return 14;}
    if (number < 1000000000000000ULL) {return 15;}
    if (number < 10000000000000000ULL) {return 16;}
    if (number < 100000000000000000ULL) {return 17;}
    if (number < 1000000000000000000ULL) {return 18;}
    if (number < 10000000000000000000ULL) {return 19;}

    return 20;
}