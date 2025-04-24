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

////////////////////////////////
// DEFINES
#if ENABLE_READABILITY
    #define READABILITY_ENDLINE             "\n"
    #define READABILITY_SPACE               " "
#else
    #define READABILITY_ENDLINE             ""
    #define READABILITY_SPACE               ""
#endif



#define FWRITE_STRING(string) {if(fwrite(string, BYTE_SIZE, SIZEOF_NOTERM(string), currentCfile_) < 0) return ERROR;}

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
static bool fileWriteMainHeader_(void);

static bool fileWriteExpression_(const VectorHandler_t expression);
static inline bool fileWriteVariablesAllocation_(const BitpackSize_t bitsize, const uint32_t scopeIndex);
static bool printBitVariableReading_(const ExpressionHandle_t operand);
static bool generateCodeForOperation_(const ExpressionHandle_t left, const ExpressionHandle_t right, const OperatorType_t operator);
static bool fileWriteBitVariableSet_(const ExpressionHandle_t left, const ExpressionHandle_t right);
////////////////////////////////
// IMPLEMENTATION

bool Generator_generateCode(const MainFrameHandle_t ast, const char* dstCFileName)
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

    // Generating class variables

    // if(!fileWriteVariablesHashmap_(&(currentAst_->classVariables)))
    // {
    //     Log_e(TAG, "Failed to write c class variables");
    //     return ERROR;
    // }

    // // Generating public methods
    if(!fileWriteMainHeader_())
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

    return SUCCESS;
}


static bool fileWriteMainHeader_(void)
{
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
    int writeStatus;
    writeStatus = fwrite(INCLUDE_WRAP("stdint") READABILITY_ENDLINE , BYTE_SIZE, SIZEOF_NOTERM(INCLUDE_WRAP("stdint") READABILITY_ENDLINE), currentCfile_);

    if(writeStatus < 0)
    {
        Log_e(TAG, "Failed to write lib \"%s\" in object:%s", "stdint", currentAst_->iguanaObjectName);
        return ERROR;
    }

    return SUCCESS;
}

static bool fileWriteMainHTypedefs_(void)
{
    const int writeStatus = fwrite(BITPACK_TYPE_TYPEDEF_DEF READABILITY_ENDLINE, BYTE_SIZE, SIZEOF_NOTERM(BITPACK_TYPE_TYPEDEF_DEF READABILITY_ENDLINE), currentCfile_);
    return (writeStatus >= 0);
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

    if(method->parameters->currentSize > 0)
    {
        for(uint32_t parameterIdx = 0; parameterIdx < method->parameters->currentSize; parameterIdx++)
        {
            const VariableObjectHandle_t varParam = (VariableObjectHandle_t) method->parameters->expandable[parameterIdx];
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

        if(!fileWriteExpression_(expression))
        {
            Log_e(TAG, "Failed to write expression");
            return ERROR;
        }
        
        
    }
    
    fwrite(READABILITY_ENDLINE BRACKET_END_DEF READABILITY_ENDLINE, BYTE_SIZE, SIZEOF_NOTERM(READABILITY_ENDLINE BRACKET_END_DEF READABILITY_ENDLINE), currentCfile_);

    return SUCCESS;
}


static bool fileWriteExpression_(const VectorHandler_t expression)
{
    DynamicStack_t symbolStack;

    Expression_t tmpVar = {EXP_TMP_VAR, STRINGIFY(TMP_VAR)};

    if(!Stack_create(&symbolStack))
    {
        Log_e(TAG, "Failed to create dynamic stack for postfix handling");
        return ERROR;
    }

    fwrite(BRACKET_START_DEF READABILITY_ENDLINE BIT_TYPE_DEF " " STRINGIFY(TMP_VAR) SEMICOLON_DEF READABILITY_ENDLINE,
        BYTE_SIZE,
        SIZEOF_NOTERM(BRACKET_START_DEF READABILITY_ENDLINE BIT_TYPE_DEF " " STRINGIFY(TMP_VAR) SEMICOLON_DEF READABILITY_ENDLINE),
        currentCfile_);

    for(size_t symbolIdx = 0; symbolIdx < expression->currentSize; symbolIdx++)
    {
        const ExpressionHandle_t symbol = (ExpressionHandle_t) expression->expandable[symbolIdx];

        if(Expression_isSymbolOperand(symbol))
        {
            Stack_push(&symbolStack, symbol);
        }else if(Expression_isSymbolOperator(symbol))
        {
            const ExpressionHandle_t right = Stack_pop(&symbolStack);
            const ExpressionHandle_t left = Stack_pop(&symbolStack);

            const OperatorType_t operand = (OperatorType_t) symbol->expressionObject;

            // DO STUFF
            if(!generateCodeForOperation_(left, right, operand))
            {
                return ERROR;
            }
   
            if(!Stack_push(&symbolStack, &tmpVar))
            {
                Log_e(TAG, "Failed to push stack expression");
                return ERROR;
            }
        }
    }

    fwrite(BRACKET_END_DEF,
        BYTE_SIZE,
        SIZEOF_NOTERM(BRACKET_END_DEF),
        currentCfile_);
    
    Stack_destroy(&symbolStack);
    
    return SUCCESS;
}

static bool generateCodeForOperation_(const ExpressionHandle_t left, const ExpressionHandle_t right, const OperatorType_t operator)
{
    char* operatorString = NULL;
    int status;

    // Set handling differently
    if(operator != OP_SET)
    {
        status = fwrite(STRINGIFY(TMP_VAR) C_OPERATOR_EQUAL_DEF, BYTE_SIZE, SIZEOF_NOTERM(STRINGIFY(TMP_VAR) C_OPERATOR_EQUAL_DEF), currentCfile_);
        
        if(!printBitVariableReading_(left))
        {
            return ERROR;
        }
    }else
    {
        return fileWriteBitVariableSet_(left, right);
    }


    switch (operator)
    {
        case OP_PLUS: operatorString = C_OPERATOR_PLUS_DEF; break;
        case OP_MINUS: operatorString = C_OPERATOR_MINUS_DEF; break;
        case OP_MULTIPLY: operatorString = C_OPERATOR_MULTIPLY_DEF; break;
        case OP_DIVIDE: operatorString = C_OPERATOR_DIVIDE_DEF; break;
        case OP_MODULUS: operatorString = C_OPERATOR_MODULUS_DEF; break;
        case OP_SET: operatorString = C_OPERATOR_EQUAL_DEF; break;

        case OP_BIN_AND: operatorString = C_OPERATOR_BIN_AND_DEF; break;
        // case OP_BIN_NOT: operatorString = C_OPERATOR_DIVIDE_DEF; break;
        case OP_BIN_OR: operatorString = C_OPERATOR_BIN_OR_DEF; break;
        case OP_BIN_XOR: operatorString = C_OPERATOR_BIN_XOR_DEF; break;

        default: operatorString = NULL; break;
    }

    status = fwrite(operatorString, BYTE_SIZE, strlen(operatorString), currentCfile_);

    if(status < 0)
    {
        Log_e(TAG, "Failed to write");
        return ERROR;
    }

    if(!printBitVariableReading_(right))
    {
        return ERROR;
    }

    status = fwrite(SEMICOLON_DEF READABILITY_ENDLINE, BYTE_SIZE, SIZEOF_NOTERM(SEMICOLON_DEF READABILITY_ENDLINE), currentCfile_);

    if(status < 0)
    {
        Log_e(TAG, "Failed to write");
        return ERROR;
    }

    return SUCCESS;
}

static bool printBitVariableReading_(const ExpressionHandle_t operand)
{
    int status;
    if (operand->type == EXP_VARIABLE)
    {
        const VariableObjectHandle_t variable = (VariableObjectHandle_t) operand->expressionObject;
 
        if(variable->bitpack < BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(AFIT_READ(s_%u[%u], %u, %lu)), 0, variable->belongToGroup, variable->posBit, variable->bitpack);
        }else if (variable->bitpack == BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(APLT_READ(s_%u[%u])), 0, variable->belongToGroup);
        }

    }else if(operand->type == EXP_TMP_VAR)
    {
        status = fprintf(currentCfile_, STRINGIFY(APLT_READ(tmp)));
    }else if(operand->type == EXP_CONST_NUMBER)
    {
        status = fprintf(currentCfile_, STRINGIFY(APLT_READ(%lu)), (AssignValue_t) operand->expressionObject);
    }

    return status > 0;
}


static bool fileWriteBitVariableSet_(const ExpressionHandle_t left, const ExpressionHandle_t right)
{
    int status;
    const VariableObjectHandle_t leftVar = (VariableObjectHandle_t) left->expressionObject;
    const VariableObjectHandle_t rightVar = (VariableObjectHandle_t) right->expressionObject;
        
    if(left->type == EXP_VARIABLE)
    {
        if(leftVar->bitpack < BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(s_%u[%u] = AFIT_RESET(s_%u[%u], %u, %lu)) READABILITY_SPACE C_OPERATOR_BIN_OR_DEF READABILITY_SPACE, 0, leftVar->belongToGroup, 0, leftVar->belongToGroup, leftVar->bitpack, leftVar->posBit, leftVar->bitpack);
        }else if (leftVar->bitpack == BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(s_%u[%u]) READABILITY_SPACE C_OPERATOR_EQUAL_DEF READABILITY_SPACE, 0, leftVar->belongToGroup);
        }else
        {
            Log_e(TAG, "Unhandled case vars cant be now bigger than %u", BIT_SIZE_BITPACK);
            return ERROR;
        }
    }
    
    if(right->type == EXP_TMP_VAR)
    {
        status = fprintf(currentCfile_, STRINGIFY(((TMP_VAR & AFIT_MASK(%lu)) << (BIT_SIZE_BITPACK - (%u + %lu)))) SEMICOLON_DEF READABILITY_ENDLINE, leftVar->bitpack, leftVar->posBit, leftVar->bitpack);
    }else if (right->type == EXP_VARIABLE)
    {
        if(rightVar->bitpack < BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(((AFIT_READ(s_%u[%u], %u, %lu) & AFIT_MASK(%lu)) << (BIT_SIZE_BITPACK - (%u + %lu)))) SEMICOLON_DEF READABILITY_ENDLINE, 0, rightVar->belongToGroup, rightVar->posBit, rightVar->bitpack, leftVar->bitpack, leftVar->posBit, leftVar->bitpack);
        }else if (rightVar->bitpack == BIT_SIZE_BITPACK)
        {
            status = fprintf(currentCfile_, STRINGIFY(((s_%u[%u] & AFIT_MASK(%lu)) << (BIT_SIZE_BITPACK - (%u + %lu)))) SEMICOLON_DEF READABILITY_ENDLINE, 0, rightVar->belongToGroup, leftVar->bitpack, leftVar->posBit, leftVar->bitpack);
        }else
        {   
            Log_e(TAG, "Unhandled case vars cant be now bigger than %u", BIT_SIZE_BITPACK);
            return ERROR;
        }
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
            fwrite(PARAM_TYPE_DEF READABILITY_SPACE FUNCTION_OBJ_NAME COMMA_DEF READABILITY_SPACE,
                BYTE_SIZE,
                SIZEOF_NOTERM(PARAM_TYPE_DEF READABILITY_SPACE FUNCTION_OBJ_NAME COMMA_DEF READABILITY_SPACE), currentCfile_);

        }
    }

    FWRITE_STRING(PARAM_TYPE_DEF READABILITY_SPACE FUNCTION_PARAM_NAME BRACKET_ROUND_END_DEF);

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