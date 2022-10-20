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

////////////////////////////////
// DEFINES


#define BYTE_SIZE                   1

#define NGUARD_HASH_RANDOM_SIZE     64
#define BYTE_SIZE_BITS              8

#define GENERATE_GUARD_PART(partName) \
    fprintf(generator->hFile, "%s ",partName); \
    generateNdefGuard_(generator); \
    fwrite(&END_LINE, BYTE_SIZE, sizeof(END_LINE), generator->hFile)

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

static bool iguanaPathToCharfilePath_(char* filepath, const char cFormatExtension); 
static bool generateNdefGuard_(const CodeGeneratorHandle_t generator);   
static bool fileWriteImports_(const CodeGeneratorHandle_t generator);    
static bool fileWriteClassVariables_(const CodeGeneratorHandle_t generator);   
static bool fileWriteVariableDeclaration_(const FILE* file, const VariableObjectHandle_t handle, const VariableDeclaration_t declareType);    
static bool fileWriteMethods_(const CodeGeneratorHandle_t generator);    
static bool initializeFileDescriptorFor_(FILE** descriptor, const char* virtualBuffer, char* path, const ImportObjectHandle_t iguanaImport, const char extension);     
static inline bool fileWriteMethodBody_(const CodeGeneratorHandle_t generator,const MethodObjectHandle_t method);

////////////////////////////////
// IMPLEMENTATION

bool Generator_initialize(CodeGeneratorHandle_t generator, const ImportObjectHandle_t iguanaImport, const MainFrameHandle_t ast)
{
    NULL_GUARD(iguanaImport, ERROR, Log_e(TAG, "iguanaFilePath got passed as NULL"));
    NULL_GUARD(ast, ERROR, Log_e(TAG, "Abstract syntax tree got passed as NULL"));

    generator->ast = ast;

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

    

    // *path = "./dksdss.c";
    // strncpy(*path, iguanaFilePath, iguanaFilePathLength);



    // if(!iguanaPathToCharfilePath_(*path, extension))
    // {
    //     Log_e(TAG, "Failed to change path to \".%c\" format of %s", extension, *path);
    //     return ERROR;
    // }

    

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

    if(!fileWriteClassVariables_(generator))
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


    fprintf(generator->hFile, "%s%c", ENDIF_KEYWORD, END_LINE);

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
        Log_e(TAG, "Failed to close file %s", generator->hFile);
        return ERROR;
    }

    if(fclose(generator->cFile))
    {
        Log_e(TAG, "Failed to close file %s", generator->hFile);
        return ERROR;
    }
    return SUCCESS;
}


/**
 * @brief Private method for generating NDEF guard for repeating libraries
 * 
 * @param[in] generator                 Generator for gathering file descriptor where to write 
 * @return                              Success state
 */
static bool generateNdefGuard_(const CodeGeneratorHandle_t generator)
{
    char hash[NGUARD_HASH_RANDOM_SIZE];
    Random_fast_srand(time(NULL));

    for(int i = 0; i < NGUARD_HASH_RANDOM_SIZE; i++)
    {
        hash[i] = Random_fast_rand() % 25 + 'A';            // making random capital letter
    }

    fwrite(hash, BYTE_SIZE, sizeof(hash), generator->hFile);
    return SUCCESS;
}

static inline bool fileWriteImports_(const CodeGeneratorHandle_t generator)
{
    ImportObjectHandle_t importObject;

    fprintf(generator->cFile, "%s \"%s.c\"%c", INCLUDE_KEYWORD, generator->hFile, END_LINE);
    // printf("%s\n",generator->hFilePath);
    // writing imports to h file
    for(size_t importIdx = 0; importIdx < (generator->ast->imports->currentSize); importIdx++)
    {
        importObject = (ImportObjectHandle_t) generator->ast->imports->expandable[importIdx];
        NULL_GUARD(importObject, ERROR, Log_e(TAG, "Import object from Abstract syntax tree is null"));
        NULL_GUARD(importObject->name, ERROR, Log_e(TAG, "Import name is null"));
        NULL_GUARD(importObject->objectId.id, ERROR, Log_e(TAG, "Import object id is null"));


        fprintf(generator->hFile, "%s \"%s.h\"%c", INCLUDE_KEYWORD, importObject->objectId.id, END_LINE);

    }
    return SUCCESS;
}


static inline bool fileWriteClassVariables_(const CodeGeneratorHandle_t generator)
{

    if(generator->ast->classVariables->currentSize != 0)
    {
        // generator for typedef struct{ variables };
        fprintf(generator->hFile, "%s %s%c", TYPEDEF_KEYWORD, STRUCT_KEYWORD, BRACKET_START);
        
        for(size_t variableIdx = 0; variableIdx < generator->ast->classVariables->currentSize; variableIdx++)
        {
            VariableObjectHandle_t variable;

            variable = generator->ast->classVariables->expandable[variableIdx];
            fileWriteVariableDeclaration_(generator->hFile, variable, VARIABLE_STRUCT);
        }
        fprintf(generator->hFile, "%c%c%c", BRACKET_END, SEMICOLON, END_LINE);
    }
    return SUCCESS;
}

/**
//  * @brief Private method for effective file extension changing which are 1 char length
//  * 
//  * e.g: ./filepath/iguana.i --> ./filepath/iguana.c
//  * e.g: ./filepath/iguana.ig --> ./filepath/iguana.c
//  * e.g: ./filepath/iguana.iguana --> ./filepath/iguana.h
//  * 
//  * @param[out] filepath                 relative path from main root folder to iguana file which being compiled
//  * @param[in] cFormatExtension          char to replace extension with (must be char and not string)
//  * @return                              Succes state 
//  */
// static bool iguanaPathToCharfilePath_(char* filepath, const char cFormatExtension)
// {
//     char* pointerDotStart = NULL;
    
//     pointerDotStart = strrchr(filepath, '.');
//     NULL_GUARD(pointerDotStart, ERROR, Log_e(TAG, "pointer to dot of filename is null"));
//     pointerDotStart[1] = cFormatExtension;
//     pointerDotStart[2] = NULL_TERMINATOR;
//     return SUCCESS;
// }

static bool fileWriteVariableDeclaration_(const FILE* file,const VariableObjectHandle_t variable,const VariableDeclaration_t declareType)
{
    char* variableTypeKeywordToUse;

    NULL_GUARD(variable, ERROR, Log_e(TAG, "AST variableDeclaration expandable is null"));


    if(variable->bitpack != 0)
    {
        variableTypeKeywordToUse = TYPE_BINDS[(variable->bitpack - 1)/ BYTE_SIZE_BITS]; 

        switch (declareType)
        {
            case VARIABLE_STRUCT:
                fprintf(file, "%s %s:%d%c", variableTypeKeywordToUse, variable->variableName, variable->bitpack, SEMICOLON);
                break;
            
            case VARIABLE_METHOD:
                fprintf(file, "%s %s%c", variableTypeKeywordToUse, variable->variableName, BRACKET_ROUND_START);
                break;
            case VARIABLE_NORMAL:
                fprintf(file, "%s %s%c", variableTypeKeywordToUse, variable->variableName, SEMICOLON);
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

    for(size_t methodIdx = 0; methodIdx < generator->ast->methods->currentSize; methodIdx++)
    {
        MethodObjectHandle_t method;
        method = (MethodObjectHandle_t) generator->ast->methods->expandable[methodIdx];                                                           // getting method by index
        NULL_GUARD(method, ERROR, Log_e(TAG, "AST methods vector expandable is null"));

        if(!fileWriteVariableDeclaration_(generator->hFile, method->returnVariable, VARIABLE_METHOD))
        {
            Log_e(TAG, "Failed to write method %s return type", method->methodName);
            return ERROR;
        }

        if(!fileWriteVariableDeclaration_(generator->cFile, method->returnVariable, VARIABLE_METHOD))
        {
            Log_e(TAG, "Failed to write method %s return type", method->methodName);
            return ERROR;
        }

        for(size_t paramIdx = 0; paramIdx < method->parameters->currentSize; paramIdx++)
        {
            VariableObjectHandle_t parameter;
            parameter = method->parameters->expandable[paramIdx];
            NULL_GUARD(parameter, ERROR, Log_e(TAG, "AST method %s %d nth is NULL", method->methodName, paramIdx));

            if(!fileWriteVariableDeclaration_(generator->hFile, parameter, VARIABLE_PARAMETER))
            {
                Log_e(TAG, "Failed to write method %s return type", method->methodName);
                return ERROR;
            }

            if(!fileWriteVariableDeclaration_(generator->cFile, parameter, VARIABLE_PARAMETER))
            {
                Log_e(TAG, "Failed to write method %s return type", method->methodName);
                return ERROR;
            }

            if((paramIdx + 1) != method->parameters->currentSize)
            {
                fwrite(&COMMA, BYTE_SIZE, sizeof(COMMA), generator->hFile);
                fwrite(&COMMA, BYTE_SIZE, sizeof(COMMA), generator->cFile);
            }

        }

        fprintf(generator->hFile, "%c%c\n", BRACKET_ROUND_END, SEMICOLON);
        fprintf(generator->cFile, "%c\n", BRACKET_ROUND_END);

        fileWriteMethodBody_(generator, method);
        // fprintf("%s", method->returnVariable)

    }
    return SUCCESS;

}





static inline bool fileWriteMethodBody_(const CodeGeneratorHandle_t generator,const MethodObjectHandle_t method)
{
    fprintf(generator->cFile, "%c%c\n", BRACKET_START, BRACKET_END);
}
