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
#include "stdio.h"
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

#define FOUT_BUFFER_LENGTH          1024
#define BYTE_SIZE                   1

#define NGUARD_HASH_RANDOM_SIZE     64
#define BYTE_SIZE_BITS              8

#define GENERATE_GUARD_PART(partName) \
    fprintf(hFile, "%s ",partName); \
    generateNdefGuard_(hFile); \
    fwrite(&END_LINE, BYTE_SIZE, sizeof(END_LINE), hFile)

////////////////////////////////
// PRIVATE CONSTANTS

static const char* TAG =                "GENERATOR";

////////////////////////////////
// PRIVATE TYPES

////////////////////////////////
// PRIVATE METHODS

bool iguanaPathToCharfilePath_(char* relativeIguanaFilePath, const char cFormatExtension);
bool generateNdefGuard_(const FILE* hFileDescriptor);
////////////////////////////////
// IMPLEMENTATION


bool Generator_generateCodeFor(const char* relativeIguanaFilePath, const MainFrameHandle_t ast)
{
    FILE* hFile;
    char writingbuffer[FOUT_BUFFER_LENGTH];

    if(!iguanaPathToCharfilePath_(relativeIguanaFilePath, 'h'))
    {
        Log_e(TAG, "Failed to change path to h format of %s", relativeIguanaFilePath);
        return false;
    }
    
    hFile = fopen(relativeIguanaFilePath, "w");
    NULL_GUARD(hFile, ERROR, Log_e(TAG, "Failed to open %s", relativeIguanaFilePath));

    setvbuf(hFile, writingbuffer, _IOFBF, FOUT_BUFFER_LENGTH); // setting virtual buffer for bufferizing file writing (better speed)

    // TODO: error checking of fwrite
    // writing ndef guard start

    GENERATE_GUARD_PART(NDEF_KEYWORD);
    GENERATE_GUARD_PART(DEF_KEYWORD);

    // writing imports to h file
    for(size_t importIdx = 0; importIdx < ast->imports->currentSize; importIdx++)
    {
        // TODO: put this to differ method for readability
        ImportObjectHandle_t importObject;

        importObject = (ImportObjectHandle_t) ast->imports->expandable[importIdx];
        NULL_GUARD(importObject, ERROR, Log_e(TAG, "Import object from Abstract syntax tree is null"));
        NULL_GUARD(importObject->name, ERROR, Log_e(TAG, "Import name is null"));

        fprintf(hFile, "%s \"%s.h\"%c", INCLUDE_KEYWORD, importObject->name, END_LINE);

    }
    
    if(ast->classVariables->currentSize != 0)
    {
        // generator for typedef struct{ variables };
        fprintf(hFile, "%s %s%c", TYPEDEF_KEYWORD, STRUCT_KEYWORD, BRACKET_START);
        
        for(size_t variableIdx = 0; variableIdx < ast->classVariables->currentSize; variableIdx++)
        {
            VariableObjectHandle_t variable;
            const char* variableTypeKeywordToUse;

            variable = ast->classVariables->expandable[variableIdx]; // getting variable by index
            NULL_GUARD(variable, ERROR, Log_e(TAG, "AST classVariables vector expandable is null"));


            if(variable->bitpack != 0)
            {
                variableTypeKeywordToUse = TYPE_BINDS[(variable->bitpack - 1)/ BYTE_SIZE_BITS]; 

                // deciding which variable better to use for bitpacked values
                fprintf(hFile, "%s %s:%d%c", variableTypeKeywordToUse, variable->variableName, variable->bitpack, SEMICOLON);
            }

        }
        fprintf(hFile, "%c%c%c", BRACKET_END, SEMICOLON, END_LINE);
    }

    fprintf(hFile, "%s%c", ENDIF_KEYWORD, END_LINE);

    if(fflush(hFile))  // flushing what is left in buffer
    {
        Log_e(TAG, "Failed to flush file %s buffer", hFile);
        return ERROR;
    }

    if(fclose(hFile))
    {
        Log_e(TAG, "Failed to close file %s", hFile);
        return ERROR;
    }

    return SUCCESS;
}


/**
 * @brief Private method for generating NDEF guard for repeating libraries
 * 
 * @param[in] hFileDescriptor           File descriptor where to write 
 * @return                              Success state
 */
bool generateNdefGuard_(const FILE* hFileDescriptor)
{
    char hash[NGUARD_HASH_RANDOM_SIZE];
    Random_fast_srand(time(NULL));

    for(int i = 0; i < NGUARD_HASH_RANDOM_SIZE; i++)
    {
        hash[i] = Random_fast_rand() % 25 + 'A';            // making random capital letter
    }

    fwrite(hash, BYTE_SIZE, sizeof(hash), hFileDescriptor);
    return SUCCESS;
}


/**
 * @brief Private method for effective file extension changing which are 1 char length
 * 
 * e.g: ./filepath/iguana.i --> ./filepath/iguana.c
 * e.g: ./filepath/iguana.ig --> ./filepath/iguana.c
 * e.g: ./filepath/iguana.iguana --> ./filepath/iguana.h
 * 
 * @param[in/out] relativeIguanaFilePath   Relative path from main root folder to iguana file which being compiled
 * @param[in] cFormatExtension          char to replace extension with (must be char and not string)
 * @return                              Succes state 
 */
bool iguanaPathToCharfilePath_(char* relativeIguanaFilePath, const char cFormatExtension)
{
    char* pointerDotStart = NULL;

    pointerDotStart = strrchr(relativeIguanaFilePath, '.');
    NULL_GUARD(pointerDotStart, ERROR, Log_e(TAG, "pointer to dot of filename is null"));
    pointerDotStart[1] = cFormatExtension;
    pointerDotStart[2] = NULL_TERMINATOR;
    return SUCCESS;
}

