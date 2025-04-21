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
#include "csyntax_database.h"
#include <queue.h>
#include "../parser/structures/expression/expressions.h"
////////////////////////////////
// DEFINES


#define BYTE_SIZE                   1

#define BYTE_SIZE_BITS              8
#define LOCAL_VARIABLES_STRUCT_NAME "l"

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
static int methodBodyVariableInitializerForIterator_(void *key, int count, void* value, void *user);
static int methodDeclarationIteratorCallback_(void *key, int count, void* value, void *user);
static int methodDefinitionIteratorCallback_(void *key, int count, void* value, void *user);
static int methodBodyVariableIteratorCallback_(void *key, int count, void* value, void *user);

static bool fileWriteVariableDeclaration_(const VariableObjectHandle_t variable, const VariableDeclaration_t declareType);
static bool fileWriteMethods_();

static bool fileWriteMethodBody_(const MethodObjectHandle_t method);
static bool handleExpressionWriting_(const ExpressionHandle_t expression);
static void handleOperatorWritingByType_(const TokenType_t type);
static bool fileWriteVariablesHashmap_(Hashmap_t* scopeVariables);
static bool generateMethodHeader_(const MethodObjectHandle_t method);

static inline uint8_t getDigitCountU64_(uint64_t number);
static bool fileWriteNameMangleMethod_(const char* const className, const MethodObjectHandle_t method, const bool isPublic);
////////////////////////////////
// IMPLEMENTATION

bool Generator_generateCode(const MainFrameHandle_t ast, const char* dstCFileName)
{
    
    NULL_GUARD(ast, ERROR, Log_e(TAG, "Null AST"));

    currentAst_ = ast;

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

static bool fileWriteNameMangleMethod_(const char* const className, const MethodObjectHandle_t method, const bool isPublic)
{
    int writeStatus;
    // TODO: optimize this so length will be somewhere stored entire generator
    size_t objectNameLen = strlen(className);
    size_t methodNameLen = strlen(method->methodName);

    writeStatus = fprintf(currentCfile_,
        //ex: asm("_ZN9wikipedia3fooEv");
        ASM_HEADER_MANGLE MANGLE_MAGIC_BYTE_DEF MANGLE_NEST_ID_DEF "%lu" BIT_DEF "%u_%s%ld%s" MANGLE_END_DEF,
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


static bool fileWriteVariablesHashmap_(Hashmap_t* scopeVariables)
{
    if(Hashmap_size(scopeVariables) != 0)
    {
        // generator for typedef struct{ variables };
        fprintf(currentCfile_, "%s %s" BRACKET_START_DEF, TYPEDEF_KEYWORD, STRUCT_KEYWORD);

        Hashmap_forEach(scopeVariables, methodBodyVariableIteratorCallback_, NULL);

        fprintf(currentCfile_, BRACKET_END_DEF "%s_t" SEMICOLON_DEF END_LINE_DEF, currentAst_->iguanaObjectName);
    }
    return SUCCESS;

}

static bool fileWriteVariableDeclaration_(const VariableObjectHandle_t variable, const VariableDeclaration_t declareType)
{
    char* variableTypeKeywordToUse;

    NULL_GUARD(variable, ERROR, Log_e(TAG, "AST variableDeclaration expandable is null"));


    if(variable->bitpack != 0)
    {
        // Checking if its not some kind of object
      
        if(variable->bitpack < ((sizeof(uint64_t) * 8) + 1))
        {
            variableTypeKeywordToUse = (char*) TYPE_BINDS[(variable->bitpack - 1)/ BYTE_SIZE_BITS]; 
        }else
        {
            Log_e(TAG, "Error occured bitpack is too big");
        }
  
        switch (declareType)
        {
            case VARIABLE_STRUCT:
                fprintf(currentCfile_, "%s %s:%lu" SEMICOLON_DEF END_LINE_DEF, variableTypeKeywordToUse, variable->objectName, variable->bitpack);
                break;
            
            case VARIABLE_METHOD:
                fprintf(currentCfile_, "void %s" BRACKET_ROUND_START_DEF, variable->objectName);
                break;
            case VARIABLE_NORMAL:
                fprintf(currentCfile_, "%s %s" SEMICOLON_DEF, variableTypeKeywordToUse, variable->objectName);
                break;
            case VARIABLE_PARAMETER:
                fprintf(currentCfile_, "%s %s", variableTypeKeywordToUse, variable->objectName);
                break;

            default:
                Log_e(TAG, "Error occured on identifying declaration enum type");
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


static inline bool fileWriteMethodBody_(const MethodObjectHandle_t method)
{
    fwrite(&BRACKET_START_CHAR, 1, 1, currentCfile_);

    if(!fileWriteVariablesHashmap_(&method->body.localVariables))
    {
        Log_e(TAG, "Failed to write method scope variables");
        return ERROR;
    }
    // writing structure Initializator
    if(Hashmap_size(&method->body.localVariables) != 0)
    {
        //fprintf(currentCfile_, "%s_t %s%c", generator->iguanaImport->objectId.id, LOCAL_VARIABLES_STRUCT_NAME, SEMICOLON_CHAR);
    }
    
    Hashmap_forEach(&method->body.localVariables, methodBodyVariableInitializerForIterator_, NULL);
    // writing variables assignings

    for(size_t expressionQ = 0; expressionQ < method->body.expressions.currentSize; expressionQ++)
    {
        QueueHandle_t expressionQueue;

        expressionQueue = method->body.expressions.expandable[expressionQ];
        if(expressionQueue == NULL)
        {
            return ERROR;
        }

        while (true)
        {
            ExpressionHandle_t expression;
            expression = Queue_dequeue(expressionQueue);
            if(expression == NULL)
            {
                break;
            }

            if(!handleExpressionWriting_(expression))
            {
                Log_e(TAG, "Failed to write expression");
                return ERROR;
            }

        }

        fwrite(&SEMICOLON_CHAR, BYTE_SIZE, 1, currentCfile_);

        Queue_destroy(expressionQueue);
        
    }
    
    fwrite(BRACKET_END_DEF END_LINE_DEF, BYTE_SIZE, 2, currentCfile_);

    return SUCCESS;
}

static int methodBodyVariableInitializerForIterator_(void *key, int count, void* value, void *user)
{
    VariableObjectHandle_t variable;

    variable = value;

    if(variable->hasAssignedValue)
    {
        fprintf(currentCfile_, "%s.%s = %ld" SEMICOLON_DEF, LOCAL_VARIABLES_STRUCT_NAME, variable->objectName, variable->assignedValue);
    }
    return SUCCESS;
}

static int methodBodyVariableIteratorCallback_(void *key, int count, void* value, void *user)
{
    VariableObjectHandle_t variable;

    variable = value;

    return fileWriteVariableDeclaration_(variable, VARIABLE_STRUCT);
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
        if(!fileWriteVariableDeclaration_(method->returnVariable, VARIABLE_METHOD))
        {
            Log_e(TAG, "Failed to write method %s return type", method->methodName);
            return ERROR;
        }
    }

    if(Hashmap_size(&currentAst_->classVariables) != 0)
    {
        if(method->containsBody)
        {
            // TODO later change this hardcoded 
            fprintf(currentCfile_, "void* o,");
        }
    }


    fprintf(currentCfile_, "void* p");
    // if(method->parameters->currentSize > 0)
    // {
    //     if(method->containsBody)
    //     {
    //         fwrite(COMMA_DEF, BYTE_SIZE, 1, currentCfile_);
    //     }

    // }

    // for(size_t paramIdx = 0; paramIdx < method->parameters->currentSize; paramIdx++)
    // {
    //     VariableObjectHandle_t parameter;
    //     parameter = method->parameters->expandable[paramIdx];
    //     NULL_GUARD(parameter, ERROR, Log_e(TAG, "AST method %s %d nth is NULL", method->methodName, paramIdx));

    //     if(method->containsBody)
    //     {
    //         if(!fileWriteVariableDeclaration_(parameter, VARIABLE_PARAMETER))
    //         {
    //             Log_e(TAG, "Failed to write method %s return type", method->methodName);
    //             return ERROR;
    //         }
    //     }

    //     if((paramIdx + 1) != method->parameters->currentSize)
    //     {
    //         fwrite(COMMA_DEF, BYTE_SIZE, 1, currentCfile_);
    //     }

    // }

    fwrite(BRACKET_ROUND_END_DEF, BYTE_SIZE, 1, currentCfile_);

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

    // if(method->returnVariable.bitpack != 0)
    // {
    //     variableTypeKeywordToUse = (char*) TYPE_BINDS[(method->returnVariable.bitpack - 1) / BYTE_SIZE_BITS]; 
        
    //     fprintf(currentCfile_,"%s %s_%s%c",
    //         variableTypeKeywordToUse,
    //         currentAst_->iguanaObjectName,
    //         method->methodName, BRACKET_ROUND_START_CHAR);
    // }else
    // {
    //     Log_e(TAG, "Failed to write method %s return type", method->methodName);
    //     return ERROR;
    // }
    
    if(!generateMethodHeader_(method))
    {
        Log_e(TAG, "Failed to generate method %s header", method->methodName);
        return ERROR;
    }

    if(method->containsBody)
    {
        fwrite(END_LINE_DEF, BYTE_SIZE, 1, currentCfile_);

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

static bool handleExpressionWriting_(const ExpressionHandle_t expression)
{
    if(expression == NULL)
    {
        Log_e(TAG, "Expression pointer is NULL");
        return ERROR;
    }

    if(expression->type == METHOD_CALL)
    {
        ExMethodCallHandle_t methodCallHandle;
        methodCallHandle = expression->expressionObject;
        if(methodCallHandle->isMethodSelf)
        {
            //fprintf(currentCfile_, "%s_%s((void*)0)",generator->iguanaImport->objectId.id, methodCallHandle->method.methodName);
        }else
        {
            // not implemented yet
        }

        free(methodCallHandle);

    }else if(expression->type == CONSTANT_NUMBER)
    {
        ConstantNumberHandle_t numberHandle;
        numberHandle = expression->expressionObject;
        NULL_GUARD(numberHandle, ERROR, Log_e(TAG, "Constant number handle is null somehow"));

        fprintf(currentCfile_, "%s", numberHandle->valueAsString);

    }else if(expression->type == OPERATOR)
    {
        OperatorHandle_t operatorHandle;
        operatorHandle = expression->expressionObject;
        NULL_GUARD(operatorHandle, ERROR, Log_e(TAG, "Operator handle is null somehow"));
        handleOperatorWritingByType_(operatorHandle->operatorTokenType);

    }else if(expression->type == VARIABLE_NAME)
    {
        char* variableName;
        variableName = expression->expressionObject;
        NULL_GUARD(variableName, ERROR, Log_e(TAG, "Variable name handle is null somehow"));

        fprintf(currentCfile_, "%s.%s", LOCAL_VARIABLES_STRUCT_NAME, variableName);
    }else
    {
        Log_e(TAG, "Unrecognised expression \'ID:%d\'", expression->type);
        return ERROR;
    }
    
    return SUCCESS;
}


static void handleOperatorWritingByType_(const TokenType_t type)
{
 
        switch (type)
        {
            case OPERATOR_PLUS:
                {
                    fprintf(currentCfile_, "+");
                }break;

            case OPERATOR_MINUS:
                {
                    fprintf(currentCfile_, "-");
                }break;

            case OPERATOR_MULTIPLY:
                {
                    fprintf(currentCfile_, "*");
                }break;
            
            case OPERATOR_DIVIDE:
                {
                    fprintf(currentCfile_, "/");
                }break;
            case OPERATOR_MODULUS:
                {
                    fprintf(currentCfile_, "%%");
                }break;
            case EQUAL:
                {
                    fprintf(currentCfile_, "=");
                }break;
            case OPERATOR_XOR:
                {
                    fprintf(currentCfile_, "^");
                }break;
            case OPERATOR_OR:
                {
                    fprintf(currentCfile_, "|");
                }break;
            case OPERATOR_AND:
            {
                fprintf(currentCfile_, "&");
            }break;
            default:break;
        }
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