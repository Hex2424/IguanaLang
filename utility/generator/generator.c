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
#include "../randomizer/random.h"
#include <time.h>
#include "csyntax_database.h"

////////////////////////////////
// DEFINES
#define LINUX_TEMP_FOLDER_PATH      "/tmp"
#define WINDOWS_TEMP_FOLDER_PATH    "ECHO %Temp%" // get from CMD

#if OS == LINUX
    #define TEMP_PATH                   LINUX_TEMP_FOLDER_PATH
#elif OS == WINDOWS
    #define TEMP_PATH                   WINDOWS_TEMP_FOLDER_PATH
#endif

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

bool iguanaPathToCharfilePath_(CodeGeneratorHandle_t generator, const char cFormatExtension);
bool generateNdefGuard_(CodeGeneratorHandle_t generator);
bool fileWriteImports_(CodeGeneratorHandle_t generator);
bool fileWriteClassVariables_(CodeGeneratorHandle_t generator);
bool fileWriteVariableDeclaration_(CodeGeneratorHandle_t generator, VariableObjectHandle_t handle, VariableDeclaration_t declareType);
bool fileWriteMethods_(CodeGeneratorHandle_t generator);
////////////////////////////////
// IMPLEMENTATION



bool Generator_initialize(CodeGeneratorHandle_t generator, const char* relativeIguanaFilePath, const MainFrameHandle_t ast)
{
    NULL_GUARD(relativeIguanaFilePath, ERROR, Log_e(TAG, "iguanaFilePath got passed as NULL"));
    NULL_GUARD(ast, ERROR, Log_e(TAG, "Abstract syntax tree got passed as NULL"));

    generator->ast = ast;
    generator->relativeIguanaFilePath = relativeIguanaFilePath;

    return SUCCESS;
}

bool Generator_generateCode(CodeGeneratorHandle_t generator)
{

    if(!iguanaPathToCharfilePath_(generator, 'h'))
    {
        Log_e(TAG, "Failed to change path to h format of %s", generator->relativeIguanaFilePath);
        return false;
    }
    
    generator->hFile = fopen(generator->relativeIguanaFilePath, "w");
    NULL_GUARD(generator->hFile, ERROR, Log_e(TAG, "Failed to open %s", generator->relativeIguanaFilePath));

    setvbuf(generator->hFile, generator->writingbuffer, _IOFBF, FOUT_BUFFER_LENGTH); // setting virtual buffer for bufferizing file writing (better speed)

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

    if(fclose(generator->hFile))
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
bool generateNdefGuard_(CodeGeneratorHandle_t generator)
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

inline bool fileWriteImports_(CodeGeneratorHandle_t generator)
{
    // TODO: put this to differ method for readability
    ImportObjectHandle_t importObject;

    // writing imports to h file
    for(size_t importIdx = 0; importIdx < generator->ast->imports->currentSize; importIdx++)
    {
        importObject = (ImportObjectHandle_t) generator->ast->imports->expandable[importIdx];
        NULL_GUARD(importObject, ERROR, Log_e(TAG, "Import object from Abstract syntax tree is null"));
        NULL_GUARD(importObject->name, ERROR, Log_e(TAG, "Import name is null"));

        fprintf(generator->hFile, "%s \"%s.h\"%c", INCLUDE_KEYWORD, importObject->name, END_LINE);

    }
    return SUCCESS;
}

inline bool fileWriteClassVariables_(CodeGeneratorHandle_t generator)
{

    if(generator->ast->classVariables->currentSize != 0)
    {
        // generator for typedef struct{ variables };
        fprintf(generator->hFile, "%s %s%c", TYPEDEF_KEYWORD, STRUCT_KEYWORD, BRACKET_START);
        
        for(size_t variableIdx = 0; variableIdx < generator->ast->classVariables->currentSize; variableIdx++)
        {
            VariableObjectHandle_t variable;

            variable = generator->ast->classVariables->expandable[variableIdx];
            fileWriteVariableDeclaration_(generator, variable, VARIABLE_STRUCT);
        }
        fprintf(generator->hFile, "%c%c%c", BRACKET_END, SEMICOLON, END_LINE);
    }
    return SUCCESS;
}

/**
 * @brief Private method for effective file extension changing which are 1 char length
 * 
 * e.g: ./filepath/iguana.i --> ./filepath/iguana.c
 * e.g: ./filepath/iguana.ig --> ./filepath/iguana.c
 * e.g: ./filepath/iguana.iguana --> ./filepath/iguana.h
 * 
 * @param[in/out] generator             generator for gathering relative path from main root folder to iguana file which being compiled
 * @param[in] cFormatExtension          char to replace extension with (must be char and not string)
 * @return                              Succes state 
 */
bool iguanaPathToCharfilePath_(CodeGeneratorHandle_t generator, const char cFormatExtension)
{
    char* pointerDotStart = NULL;

    pointerDotStart = strrchr(generator->relativeIguanaFilePath, '.');
    NULL_GUARD(pointerDotStart, ERROR, Log_e(TAG, "pointer to dot of filename is null"));
    pointerDotStart[1] = cFormatExtension;
    pointerDotStart[2] = NULL_TERMINATOR;
    return SUCCESS;
}

bool fileWriteVariableDeclaration_(CodeGeneratorHandle_t generator, VariableObjectHandle_t variable, VariableDeclaration_t declareType)
{
    char* variableTypeKeywordToUse;

    NULL_GUARD(variable, ERROR, Log_e(TAG, "AST classVariables vector expandable is null"));


    if(variable->bitpack != 0)
    {
        variableTypeKeywordToUse = TYPE_BINDS[(variable->bitpack - 1)/ BYTE_SIZE_BITS]; 

        switch (declareType)
        {
            case VARIABLE_STRUCT:
                fprintf(generator->hFile, "%s %s:%d%c", variableTypeKeywordToUse, variable->variableName, variable->bitpack, SEMICOLON);
                break;
            
            case VARIABLE_METHOD:
                fprintf(generator->hFile, "%s %s%c", variableTypeKeywordToUse, variable->variableName, BRACKET_ROUND_START);
                break;
            case VARIABLE_NORMAL:
                fprintf(generator->hFile, "%s %s%c", variableTypeKeywordToUse, variable->variableName, SEMICOLON);
                break;
            case VARIABLE_PARAMETER:
                fprintf(generator->hFile, "%s %s", variableTypeKeywordToUse, variable->variableName);
                break;

            default:
                Log_e(TAG, "Error occured on identifying declaration enum type");
                return ERROR;
        }
    }
}



inline bool fileWriteMethods_(CodeGeneratorHandle_t generator)
{
    NULL_GUARD(generator, ERROR, Log_e(TAG, "Generator to method writing passes as null"));

    for(size_t methodIdx = 0; methodIdx < generator->ast->methods->currentSize; methodIdx++)
    {
        MethodObjectHandle_t method;
        method = (MethodObjectHandle_t) generator->ast->methods->expandable[methodIdx];                                                           // getting method by index
        NULL_GUARD(method, ERROR, Log_e(TAG, "AST methods vector expandable is null"));

        if(!fileWriteVariableDeclaration_(generator, method->returnVariable, VARIABLE_METHOD))
        {
            Log_e(TAG, "Failed to write method %s return type", method->methodName);
            return ERROR;
        }

        for(size_t paramIdx = 0; paramIdx < method->parameters->currentSize; paramIdx)
        {
            if(!fileWriteVariableDeclaration_(generator, method->returnVariable, VARIABLE_PARAMETER))
            {
                Log_e(TAG, "Failed to write method %s return type", method->methodName);
                return ERROR;
            }
            if(paramIdx + 1 != method->parameters->currentSize)
            {
                fwrite(COMMA, BYTE_SIZE, sizeof(COMMA), generator->hFile);
            }
        }

        fprintf(generator->hFile, "%c%c", BRACKET_ROUND_END, SEMICOLON);
        // fprintf("%s", method->returnVariable)

    }
    return SUCCESS;

}