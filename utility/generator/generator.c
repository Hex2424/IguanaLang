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
#include "../vector/vector.h"
#include "../global_config/global_config.h"
#include "string.h"
#include "../logger/logger.h"
#include "../hash/hash.h"
#include <time.h>
#include "csyntax_database.h"
#include "../queue/queue.h"
#include "../parser/structures/expression/expressions.h"
////////////////////////////////
// DEFINES


#define BYTE_SIZE                   1

#define NGUARD_HASH_RANDOM_SIZE     64
#define BYTE_SIZE_BITS              8
#define LOCAL_VARIABLES_STRUCT_NAME "l"


#define GENERATE_GUARD_PART(partName) \
    fprintf(generator->hFile, "%s ",partName); \
    fprintf(generator->hFile, "DEFGUARD_%s",generator->iguanaImport->objectId.id); \
    fwrite(&END_LINE_CHAR, BYTE_SIZE, sizeof(END_LINE_CHAR), generator->hFile)

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

////////////////////////////////
// PRIVATE METHODS

// iterator callbacks
static int methodBodyVariableInitializerForIterator_(void *key, int count, void* value, void *user);
static int methodDeclarationIteratorCallback_(void *key, int count, void* value, void *user);
static int classImportsIteratorCallback(void *key, int count, void* value, void *user);
static int methodBodyVariableIteratorCallback_(void *key, int count, void* value, void *user);

static char* filePathToModulePath_(const CodeGeneratorHandle_t generator);
static bool iguanaPathToCharfilePath_(char* filepath, const char cFormatExtension); 
static bool generateNdefGuard_(const CodeGeneratorHandle_t generator);   
static bool fileWriteImports_(const CodeGeneratorHandle_t generator);    
static bool fileWriteVariableDeclaration_(const FILE* file, const ImportObjectHandle_t currentObjectId, const VariableObjectHandle_t variable, const VariableDeclaration_t declareType);
static bool fileWriteMethods_(const CodeGeneratorHandle_t generator);    
static bool initializeFileDescriptorFor_(FILE** descriptor, const char* virtualBuffer, char* path, const ImportObjectHandle_t iguanaImport, const char extension);     
static inline bool fileWriteMethodBody_(const CodeGeneratorHandle_t generator,const MethodObjectHandle_t method);
static bool handleExpressionWriting_(const CodeGeneratorHandle_t generator,const ExpressionHandle_t expression);
static void handleOperatorWritingByType_(FILE* file, const TokenType_t type);
static inline bool fileWriteVariablesHashmap_(FILE* file, const HashmapHandle_t scopeVariablesHashmap, const ImportObjectHandle_t currentFileObjectId);
////////////////////////////////
// IMPLEMENTATION

bool Generator_initialize(CodeGeneratorHandle_t generator, const ImportObjectHandle_t iguanaImport, const MainFrameHandle_t ast)
{
    NULL_GUARD(iguanaImport, ERROR, Log_e(TAG, "iguanaFilePath got passed as NULL"));
    NULL_GUARD(ast, ERROR, Log_e(TAG, "Abstract syntax tree got passed as NULL"));

    generator->ast = ast;
    generator->iguanaImport = iguanaImport;

    if(!initializeFileDescriptorFor_(&generator->hFile, generator->writingBufferH, &generator->hFilePath, iguanaImport, 'h'))
    {
        return ERROR;
    }
    
    if(!initializeFileDescriptorFor_(&generator->cFile, generator->writingBufferC, generator->cFilePath, iguanaImport, 'c'))
    {
        return ERROR;
    }

    return SUCCESS;
}


static bool initializeFileDescriptorFor_(FILE** descriptor,const char* virtualBuffer, char* path, const ImportObjectHandle_t iguanaFilePath, const char extension)
{
    memcpy(path, TEMP_PATH, sizeof(TEMP_PATH) - 1);
    memcpy(path + sizeof(TEMP_PATH) - 1, iguanaFilePath->objectId.id, OBJECT_ID_LENGTH);
    path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH) + 1] = '\0';
    path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH)] = extension;
    path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH) - 1] = '.';

    if(path == NULL)
    {
        Log_e(TAG, "Failed to allocate memory for \".%c\" file paths for ig: %s", extension, iguanaFilePath);
        return ERROR;
    }

    *descriptor = fopen(path, "w");

    NULL_GUARD(*descriptor, ERROR, Log_e(TAG, "Failed to open %s", path));
    setvbuf(*descriptor, virtualBuffer, _IOFBF, FOUT_BUFFER_LENGTH);
    return SUCCESS;
}

bool Generator_generateCode(const CodeGeneratorHandle_t generator)
{

    // setting virtual buffer for bufferizing file writing (better speed)

    // TODO: error checking of fwrite
    // writing ndef guard start

    GENERATE_GUARD_PART(NDEF_KEYWORD);
    GENERATE_GUARD_PART(DEF_KEYWORD);


    // Generating object imports

    if(!fileWriteImports_(generator))
    {
        Log_e(TAG, "Failed to write c file imports");
        return ERROR;
    }

    // Generating class variables

    if(!fileWriteVariablesHashmap_(generator->hFile, &generator->ast->classVariables, generator->iguanaImport))
    {
        Log_e(TAG, "Failed to write c class variables");
        return ERROR;
    }

    // Generating public methods

    if(!fileWriteMethods_(generator))
    {
        Log_e(TAG, "Failed to write c class methods");
        return ERROR;
    }


    fprintf(generator->hFile, "%s%c", ENDIF_KEYWORD, END_LINE_CHAR);

    if(fflush(generator->hFile))  // flushing what is left in buffer
    {
        Log_e(TAG, "Failed to flush file %s buffer", generator->hFile);
        return ERROR;
    }

    if(fflush(generator->cFile))  // flushing what is left in buffer
    {
        Log_e(TAG, "Failed to flush file %s buffer", generator->cFile);
        return ERROR;
    }

    if(fclose(generator->hFile))
    {
        Log_w(TAG, "Failed to close file %s", generator->hFile);
    }

    if(fclose(generator->cFile))
    {
        Log_w(TAG, "Failed to close file %s", generator->hFile);
    }

    return SUCCESS;
}

static inline bool fileWriteImports_(const CodeGeneratorHandle_t generator)
{
    ImportObjectHandle_t importObject;

    fprintf(generator->cFile, "%s \"%s\"%c", INCLUDE_KEYWORD, generator->hFilePath, END_LINE_CHAR);
    // printf("%s\n",generator->hFilePath);
    // writing imports to h file
    Hashmap_forEach(&generator->ast->imports, classImportsIteratorCallback, generator);

    return SUCCESS;
}


static inline bool fileWriteVariablesHashmap_(FILE* file, const HashmapHandle_t scopeVariablesHashmap, const ImportObjectHandle_t currentObjectAsImport)
{
    if(Hashmap_size(scopeVariablesHashmap) != 0)
    {
        // generator for typedef struct{ variables };
        fprintf(file, "%s %s%c", TYPEDEF_KEYWORD, STRUCT_KEYWORD, BRACKET_START_CHAR);

        typedef struct
        {
            FILE* file;
            ImportObjectHandle_t currentObjectAsImport;
        }pack_t;

        pack_t tempPack = {file, scopeVariablesHashmap};

        Hashmap_forEach(scopeVariablesHashmap, methodBodyVariableIteratorCallback_, &tempPack);

        fprintf(file, "%c%s_t%c%c", BRACKET_END_CHAR, currentObjectAsImport->objectId.id, SEMICOLON_CHAR, END_LINE_CHAR);
    }
    return SUCCESS;

}

static bool fileWriteVariableDeclaration_(const FILE* file, const ImportObjectHandle_t currentObjectImport, const VariableObjectHandle_t variable, const VariableDeclaration_t declareType)
{
    char* variableTypeKeywordToUse;

    NULL_GUARD(variable, ERROR, Log_e(TAG, "AST variableDeclaration expandable is null"));


    if(variable->bitpack != 0)
    {
        variableTypeKeywordToUse = TYPE_BINDS[(variable->bitpack - 1)/ BYTE_SIZE_BITS]; 

        switch (declareType)
        {
            case VARIABLE_STRUCT:
                fprintf(file, "%s %s:%d%c", variableTypeKeywordToUse, variable->variableName, variable->bitpack, SEMICOLON_CHAR);
                break;
            
            case VARIABLE_METHOD:

                fprintf(file,"%s ", variableTypeKeywordToUse);
                fwrite(currentObjectImport->objectId.id, 1, OBJECT_ID_LENGTH, file);
                fprintf(file, "_%s%c", variable->variableName, BRACKET_ROUND_START_CHAR);
                break;
            case VARIABLE_NORMAL:
                fprintf(file, "%s %s%c", variableTypeKeywordToUse, variable->variableName, SEMICOLON_CHAR);
                break;
            case VARIABLE_PARAMETER:
                fprintf(file, "%s %s", variableTypeKeywordToUse, variable->variableName);
                break;

            default:
                Log_e(TAG, "Error occured on identifying declaration enum type");
                return ERROR;
        }
    }
    return SUCCESS;
}



static inline bool fileWriteMethods_(const CodeGeneratorHandle_t generator)
{
    NULL_GUARD(generator, ERROR, Log_e(TAG, "Generator to method writing passes as null"));
    Hashmap_forEach(&generator->ast->methods, methodDeclarationIteratorCallback_, generator);

    return SUCCESS;

}


static inline bool fileWriteMethodBody_(const CodeGeneratorHandle_t generator, const MethodObjectHandle_t method)
{
    fwrite(&BRACKET_START_CHAR, 1, 1, generator->cFile);

    if(!fileWriteVariablesHashmap_(generator->cFile, &method->body.localVariables, generator->iguanaImport))
    {
        Log_e(TAG, "Failed to write method scope variables");
        return ERROR;
    }
    // writing structure Initializator
    if(Hashmap_size(&method->body.localVariables) != 0)
    {
        fprintf(generator->cFile, "%s_t %s%c", generator->iguanaImport->objectId.id, LOCAL_VARIABLES_STRUCT_NAME, SEMICOLON_CHAR);
    }
    
    Hashmap_forEach(&method->body.localVariables, methodBodyVariableInitializerForIterator_, generator);
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

            if(!handleExpressionWriting_(generator, expression))
            {
                Log_e(TAG, "Failed to write expression");
                return ERROR;
            }

        }
        fprintf(generator->cFile, "%c", SEMICOLON_CHAR);
        
        Queue_destroy(expressionQueue);
        
    }
    fprintf(generator->cFile, "%c\n", BRACKET_END_CHAR);
    return SUCCESS;
}

static int methodBodyVariableInitializerForIterator_(void *key, int count, void* value, void *user)
{
    VariableObjectHandle_t variable;
    CodeGeneratorHandle_t generator;
    variable = value;
    generator = user;

    if(variable->hasAssignedValue)
    {
        fprintf(generator->cFile, "%s.%s = %s%c", LOCAL_VARIABLES_STRUCT_NAME, variable->variableName,variable->assignedValue, SEMICOLON_CHAR);
    }
    return SUCCESS;
}

static int methodBodyVariableIteratorCallback_(void *key, int count, void* value, void *user)
{
    typedef struct
    {
        FILE* file;
        ImportObjectHandle_t currentObjectAsImport;
    }pack_t;

    pack_t* tempPack;
    VariableObjectHandle_t variable;
    CodeGeneratorHandle_t generator;
    variable = value;
    tempPack = user;
    
    fileWriteVariableDeclaration_(tempPack->file, tempPack->currentObjectAsImport, variable, VARIABLE_STRUCT);
    return SUCCESS;
}

static int classImportsIteratorCallback(void *key, int count, void* value, void *user)
{
    ImportObjectHandle_t importObject;
    CodeGeneratorHandle_t generator;
    importObject = value;
    generator = user;

    NULL_GUARD(importObject, ERROR, Log_e(TAG, "Import object from Abstract syntax tree is null"));
    NULL_GUARD(importObject->name, ERROR, Log_e(TAG, "Import name is null"));
    NULL_GUARD(importObject->objectId.id, ERROR, Log_e(TAG, "Import object id is null"));

    fprintf(generator->hFile, "%s \"%s.h\"%c", INCLUDE_KEYWORD, importObject->objectId.id, END_LINE_CHAR);
    return SUCCESS;
}

static int methodDeclarationIteratorCallback_(void *key, int count, void* value, void *user)
{
    MethodObjectHandle_t method;
    CodeGeneratorHandle_t generator;
    char* variableTypeKeywordToUse;
    method = value;
    generator = user;

    NULL_GUARD(method, ERROR, Log_e(TAG, "AST method '%s' is NULL", key));

    if(method->accessType == IGNORED)
    {
        return SUCCESS;
    }

    if(&method->returnVariable.bitpack != 0)
    {
        variableTypeKeywordToUse = TYPE_BINDS[(method->returnVariable.bitpack - 1)/ BYTE_SIZE_BITS]; 
        fprintf(generator->hFile,"%s %s_%s%c",
            variableTypeKeywordToUse,
            generator->iguanaImport->objectId.id,
            method->methodName, BRACKET_ROUND_START_CHAR);
    }else
    {
        Log_e(TAG, "Failed to write method %s return type", method->methodName);
        return ERROR;
    }
    
    if(method->containsBody)
    {
        if(!fileWriteVariableDeclaration_(generator->cFile, generator->iguanaImport, &method->returnVariable, VARIABLE_METHOD))
        {
            Log_e(TAG, "Failed to write method %s return type", method->methodName);
            return ERROR;
        }
    }

    if(Hashmap_size(&generator->ast->classVariables) != 0)
    {
        if(method->containsBody)
        {
            fprintf(generator->cFile, "%s_t* root", generator->iguanaImport->objectId.id);
        }
        fprintf(generator->hFile, "%s_t* root", generator->iguanaImport->objectId.id);
    }
    
    if(method->parameters->currentSize > 0)
    {
        if(method->containsBody)
        {
            fwrite(&COMMA_CHAR, 1, 1, generator->cFile);
        }

        fwrite(&COMMA_CHAR, 1, 1, generator->hFile);
    }

    for(size_t paramIdx = 0; paramIdx < method->parameters->currentSize; paramIdx++)
    {
        VariableObjectHandle_t parameter;
        parameter = method->parameters->expandable[paramIdx];
        NULL_GUARD(parameter, ERROR, Log_e(TAG, "AST method %s %d nth is NULL", method->methodName, paramIdx));

        if(!fileWriteVariableDeclaration_(generator->hFile,generator->iguanaImport, parameter, VARIABLE_PARAMETER))
        {
            Log_e(TAG, "Failed to write method %s return type", method->methodName);
            return ERROR;
        }

        if(method->containsBody)
        {
            if(!fileWriteVariableDeclaration_(generator->cFile,generator->iguanaImport, parameter, VARIABLE_PARAMETER))
            {
                Log_e(TAG, "Failed to write method %s return type", method->methodName);
                return ERROR;
            }
        }

        if((paramIdx + 1) != method->parameters->currentSize)
        {
            fwrite(&COMMA_CHAR, BYTE_SIZE, sizeof(COMMA_CHAR), generator->hFile);
            fwrite(&COMMA_CHAR, BYTE_SIZE, sizeof(COMMA_CHAR), generator->cFile);
        }

    }

    fprintf(generator->hFile, "%c asm (\"%s_%s\")%c\n",
        BRACKET_ROUND_END_CHAR,
        filePathToModulePath_(generator),
        method->methodName,SEMICOLON_CHAR);
    
    
    if(method->containsBody)
    {
        fprintf(generator->cFile, "%c\n", BRACKET_ROUND_END_CHAR);
        if(!fileWriteMethodBody_(generator, method))
        {
            Log_e(TAG, "Failed to write method body to IO");
            return ERROR;
        }
    }else
    {
        // TODO abstracting fwrites
        fwrite(&SEMICOLON_CHAR, 1, 1, generator->cFile);
    }
    return SUCCESS;
}


static char* filePathToModulePath_(const CodeGeneratorHandle_t generator)
{
    char* modulePath;
    char* tempOffset;
    uint32_t realPathLength;
    uint32_t iterator;

    iterator = 0;


    // TODO optimize these iterators
    while (generator->compiler->mainIguanaFilePath[iterator] == generator->iguanaImport->realPath[iterator] &&
        generator->compiler->mainIguanaFilePath[iterator] != NULL &&
        generator->iguanaImport->realPath[iterator] != NULL 
    )
    {
        if(generator->iguanaImport->realPath[iterator] == '/')
        {
            tempOffset = generator->iguanaImport->realPath + iterator;
        }
        iterator++;

        /* code */
    }

    tempOffset++;

    realPathLength = strlen(tempOffset);

    for(char* backwardsIterator = (tempOffset + realPathLength - 1); backwardsIterator >= tempOffset; backwardsIterator--)
    {
        realPathLength--;
        if((*backwardsIterator) == '.')
        {
            break;
        }
    }

    ALLOC_CHECK(modulePath, realPathLength + 1, ERROR);

    for(uint32_t pos = 0; pos < realPathLength; pos++)
    {
        char symbol;
        if((symbol = tempOffset[pos]) == '/')
        {
            symbol = '.';
        }
        
        modulePath[pos] = symbol;
    }
    modulePath[realPathLength] = '\0';

    return modulePath;
}

static bool handleExpressionWriting_(const CodeGeneratorHandle_t generator, const ExpressionHandle_t expression)
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
            fprintf(generator->cFile, "%s_%s((void*)0)",generator->iguanaImport->objectId.id, methodCallHandle->method.methodName);
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

        fprintf(generator->cFile, "%s", numberHandle->valueAsString);

    }else if(expression->type == OPERATOR)
    {
        OperatorHandle_t operatorHandle;
        operatorHandle = expression->expressionObject;
        NULL_GUARD(operatorHandle, ERROR, Log_e(TAG, "Operator handle is null somehow"));
        handleOperatorWritingByType_(generator->cFile, operatorHandle->operatorTokenType);

    }else if(expression->type == VARIABLE_NAME)
    {
        char* variableName;
        variableName = expression->expressionObject;
        NULL_GUARD(variableName, ERROR, Log_e(TAG, "Variable name handle is null somehow"));

        fprintf(generator->cFile, "%s.%s", LOCAL_VARIABLES_STRUCT_NAME, variableName);
    }else
    {
        Log_e(TAG, "Unrecognised expression \'ID:%d\'", expression->type);
        return ERROR;
    }
    
    return SUCCESS;
}


static void handleOperatorWritingByType_(FILE* file, const TokenType_t type)
{
 
        switch (type)
        {
        case OPERATOR_PLUS:
            {
                fprintf(file, "+");
            }break;

        case OPERATOR_MINUS:
            {
                fprintf(file, "-");
            }break;

        case OPERATOR_MULTIPLY:
            {
                fprintf(file, "*");
            }break;
        
        case OPERATOR_DIVIDE:
            {
                fprintf(file, "/");
            }break;
        case OPERATOR_MODULUS:
            {
                fprintf(file, "%%");
            }break;
        case EQUAL:
            {
                fprintf(file, "=");
            }break;
        case OPERATOR_XOR:
            {
                fprintf(file, "^");
            }break;
        case OPERATOR_OR:
            {
                fprintf(file, "|");
            }break;
        case OPERATOR_AND:
        {
            fprintf(file, "&");
        }break;
        default:
            break;
        }
}