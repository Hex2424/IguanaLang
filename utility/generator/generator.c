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


// privateTypes
static MainFrameHandle_t currentAst_ =   NULL;
static FILE* currentCfile_ =             NULL;

static char writingBufferC_[FOUT_BUFFER_LENGTH];

////////////////////////////////
// PRIVATE METHODS

// iterator callbacks
static int methodBodyVariableInitializerForIterator_(void *key, int count, void* value, void *user);
static int methodDeclarationIteratorCallback_(void *key, int count, void* value, void *user);
static int methodBodyVariableIteratorCallback_(void *key, int count, void* value, void *user);

static bool fileWriteVariableDeclaration_(const VariableObjectHandle_t variable, const VariableDeclaration_t declareType);
static bool fileWriteMethods_();    
static bool fileWriteMethodBody_(const MethodObjectHandle_t method);
static bool handleExpressionWriting_(const ExpressionHandle_t expression);
static void handleOperatorWritingByType_(const TokenType_t type);
static bool fileWriteVariablesHashmap_(Hashmap_t* scopeVariables);
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

    if(!fileWriteVariablesHashmap_(&(currentAst_->classVariables)))
    {
        Log_e(TAG, "Failed to write c class variables");
        return ERROR;
    }

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

static bool fileWriteVariablesHashmap_(Hashmap_t* scopeVariables)
{
    if(Hashmap_size(scopeVariables) != 0)
    {
        // generator for typedef struct{ variables };
        fprintf(currentCfile_, "%s %s%c", TYPEDEF_KEYWORD, STRUCT_KEYWORD, BRACKET_START_CHAR);

        Hashmap_forEach(scopeVariables, methodBodyVariableIteratorCallback_, NULL);

        fprintf(currentCfile_, "%c%s_t%c%c", BRACKET_END_CHAR, currentAst_->iguanaObjectName, SEMICOLON_CHAR, END_LINE_CHAR);
    }
    return SUCCESS;

}

static bool fileWriteVariableDeclaration_(const VariableObjectHandle_t variable, const VariableDeclaration_t declareType)
{
    char* variableTypeKeywordToUse;

    NULL_GUARD(variable, ERROR, Log_e(TAG, "AST variableDeclaration expandable is null"));


    if(variable->bitpack != 0)
    {
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
                fprintf(currentCfile_, "%s %s:%d%c", variableTypeKeywordToUse, variable->variableName, variable->bitpack, SEMICOLON_CHAR);
                break;
            
            case VARIABLE_METHOD:

                fprintf(currentCfile_,"%s ", variableTypeKeywordToUse);
                fwrite(currentAst_->iguanaObjectName, 1, OBJECT_ID_LENGTH, currentCfile_);
                fprintf(currentCfile_, "_%s%c", variable->variableName, BRACKET_ROUND_START_CHAR);
                break;
            case VARIABLE_NORMAL:
                fprintf(currentCfile_, "%s %s%c", variableTypeKeywordToUse, variable->variableName, SEMICOLON_CHAR);
                break;
            case VARIABLE_PARAMETER:
                fprintf(currentCfile_, "%s %s", variableTypeKeywordToUse, variable->variableName);
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
        fprintf(currentCfile_, "%c", SEMICOLON_CHAR);
        
        Queue_destroy(expressionQueue);
        
    }
    fprintf(currentCfile_, "%c\n", BRACKET_END_CHAR);
    return SUCCESS;
}

static int methodBodyVariableInitializerForIterator_(void *key, int count, void* value, void *user)
{
    VariableObjectHandle_t variable;

    variable = value;

    if(variable->hasAssignedValue)
    {
        fprintf(currentCfile_, "%s.%s = %ld%c", LOCAL_VARIABLES_STRUCT_NAME, variable->variableName, variable->assignedValue, SEMICOLON_CHAR);
    }
    return SUCCESS;
}

static int methodBodyVariableIteratorCallback_(void *key, int count, void* value, void *user)
{
    VariableObjectHandle_t variable;

    variable = value;

    fileWriteVariableDeclaration_(variable, VARIABLE_STRUCT);
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
    
    if(method->containsBody)
    {
        if(!fileWriteVariableDeclaration_(&method->returnVariable, VARIABLE_METHOD))
        {
            Log_e(TAG, "Failed to write method %s return type", method->methodName);
            return ERROR;
        }
    }


    if(Hashmap_size(&currentAst_->classVariables) != 0)
    {
        if(method->containsBody)
        {
            fprintf(currentCfile_, "%s_t* root", currentAst_->iguanaObjectName);
        }
    }
 
    if(method->parameters->currentSize > 0)
    {
        if(method->containsBody)
        {
            fwrite(&COMMA_CHAR, 1, 1, currentCfile_);
        }

    }

    for(size_t paramIdx = 0; paramIdx < method->parameters->currentSize; paramIdx++)
    {
        VariableObjectHandle_t parameter;
        parameter = method->parameters->expandable[paramIdx];
        NULL_GUARD(parameter, ERROR, Log_e(TAG, "AST method %s %d nth is NULL", method->methodName, paramIdx));

        // if(!fileWriteVariableDeclaration_(parameter, VARIABLE_PARAMETER))
        // {
        //     Log_e(TAG, "Failed to write method %s return type", method->methodName);
        //     return ERROR;
        // }

        if(method->containsBody)
        {
            if(!fileWriteVariableDeclaration_(parameter, VARIABLE_PARAMETER))
            {
                Log_e(TAG, "Failed to write method %s return type", method->methodName);
                return ERROR;
            }
        }

        if((paramIdx + 1) != method->parameters->currentSize)
        {
            fwrite(&COMMA_CHAR, BYTE_SIZE, sizeof(COMMA_CHAR), currentCfile_);
        }

    }

    // fprintf(generator->hFile, "%casm(\"%s___%s\")%c\n",
    //     BRACKET_ROUND_END_CHAR,
    //     filePathToModulePath_(generator),
    //     method->methodName,SEMICOLON_CHAR);
    
    
    if(method->containsBody)
    {
        fprintf(currentCfile_, "%c\n", BRACKET_ROUND_END_CHAR);
        if(!fileWriteMethodBody_(method))
        {
            Log_e(TAG, "Failed to write method body to IO");
            return ERROR;
        }
    }else
    {
        // TODO abstracting fwrites
        fwrite(&SEMICOLON_CHAR, 1, 1, currentCfile_);
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