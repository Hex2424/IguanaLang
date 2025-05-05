/**
 * @file c_compiler.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-21
 */

#include "c_compiler.h"
#include "string.h"
#include "stdio.h"
#include "../../misc/safety_macros.h"
#include "../../logger/logger.h"
#include "c_compiler_macros.h"

////////////////////////////////
// DEFINES


#define GCC_COMPILER_COMMAND        "gcc -c %s -o %s.o "WARNING_SUPRESSED LOG_PATH_TO_FILE


#define GCC_COMPILER_COMMAND_COMPILE_LINK "gcc -o %s -Wl,--entry=entry_main -nostartfiles "
#define GCC_COMPILER_COMMAND_COMPILE      "gcc -c -Wl,--entry=entry_main -nostartfiles "


#define FULL_COMMAND_LEN     sizeof(GCC_COMPILER_COMMAND) + CFILES_LENGTH + CFILES_LENGTH
////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "C_COMPILER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION



// bool CExternalCompiler_compile(const char* filename, const bool removeSourceAfter)
// {
//     int closeStatus = true;

//     char full_command[FULL_COMMAND_LEN] = {0};
//     char check_command[CFILES_LENGTH + sizeof(TEMP_PATH)] = {0};
//     char sourceFilename[MAX_FILENAME_LENGTH + sizeof(TEMP_PATH)] = {0};

//     snprintf(sourceFilename, MAX_FILENAME_LENGTH + sizeof(TEMP_PATH), "%s%s.c", TEMP_PATH, filename);
    
//     snprintf(check_command, CFILES_LENGTH, "%s%s.o", TEMP_PATH, filename);

//     snprintf(full_command, FULL_COMMAND_LEN, GCC_COMPILER_COMMAND, sourceFilename, filename);
//     Log_d(TAG, "Executing command:%s", full_command);

//     if(system(full_command) == -1)
//     {
//         return ERROR;
//     }

//     FILE* objectFile = fopen(check_command, "r");

//     NULL_GUARD(objectFile, ERROR, Log_e(TAG, "Failed to compile one of c files"));

//     int rmStatus = remove(sourceFilename);

//     if (rmStatus != 0) 
//     {
//         Log_w(TAG, "Failed to remove source C file %d", rmStatus);
//         return ERROR;
//     }
        
//     closeStatus = fclose(objectFile);
    
//     if(closeStatus != 0)
//     {
//         Log_w(TAG, "Failed to close Object file at runtime: %d", closeStatus);
//         return ERROR;
//     }

//     Log_i(TAG, "Compiled C files successfuly!");
//     return SUCCESS;
// }

// bool CExternalCompiler_link(const VectorHandler_t objectsCompiledExternaly, char* outputName)
// {
//     char* full_command;
//     char* iterator;
//     ALLOC_CHECK(full_command, sizeof(GCC_COMPILER_COMMAND_LINKER) + CFILES_LENGTH + (CFILES_LENGTH * objectsCompiledExternaly->currentSize), ERROR);
//     full_command[0] = '\0'; //null terminator beggining for strncat
//     iterator = full_command;

    
//     iterator += sprintf(full_command, GCC_COMPILER_COMMAND_LINKER, outputName);

//     Log_d(TAG, "Executing command:%s", full_command);

//     for(size_t idx = 0; idx < objectsCompiledExternaly->currentSize; idx++)
//     {
//         const char* iguanaFileobject = objectsCompiledExternaly->expandable[idx];

//         NULL_GUARD(iguanaFileobject, ERROR, Log_e(TAG, "Null object in linker passed"));

//         iterator += sprintf(iterator, " %s%s.o", TEMP_PATH, iguanaFileobject);
//     }
//     Log_d(TAG, "Executing GCC inbuilt Linker: %s", full_command);

//     if(system(full_command) == -1)
//     {
//         free(full_command);
//         return ERROR;
//     }

//     Log_i(TAG, "Linked objecti mean with c compiler should have path construs successfuly!");
//     free(full_command);

//     return SUCCESS;
// }


bool CExternalCompiler_compile(const VectorHandler_t objectsCompiledExternaly, char* outputName, const bool linkingEnabled)
{
    char* full_command;
    char* iterator;
    if(linkingEnabled)
    {
        ALLOC_CHECK(full_command, sizeof(GCC_COMPILER_COMMAND_COMPILE_LINK) + CFILES_LENGTH + (CFILES_LENGTH * objectsCompiledExternaly->currentSize), ERROR);
    }else
    {
        ALLOC_CHECK(full_command, sizeof(GCC_COMPILER_COMMAND_COMPILE) + CFILES_LENGTH + (CFILES_LENGTH * objectsCompiledExternaly->currentSize), ERROR);
    }
    
    full_command[0] = '\0'; //null terminator beggining for strncat
    iterator = full_command;

    if(linkingEnabled)
    {
        iterator += sprintf(full_command, GCC_COMPILER_COMMAND_COMPILE_LINK, outputName);
    }else
    {
        iterator += sprintf(full_command, GCC_COMPILER_COMMAND_COMPILE);
    }
    
    Log_d(TAG, "Executing command:%s", full_command);

    for(size_t idx = 0; idx < objectsCompiledExternaly->currentSize; idx++)
    {
        const char* iguanaFileobject = objectsCompiledExternaly->expandable[idx];

        NULL_GUARD(iguanaFileobject, ERROR, Log_e(TAG, "Null object in linker passed"));

        iterator += sprintf(iterator, " %s%s.c", TEMP_PATH, iguanaFileobject);
    }

    if(linkingEnabled)
    {
        Log_d(TAG, "Executing GCC inbuilt compiler/Linker: %s", full_command);
    }else
    {
        Log_d(TAG, "Executing GCC compiler: %s", full_command);  
    }
    
    if(system(full_command) == -1)
    {
        free(full_command);
        return ERROR;
    }

    for(size_t idx = 0; idx < objectsCompiledExternaly->currentSize; idx++)
    {
        char sourceFilename[MAX_FILENAME_LENGTH + sizeof(TEMP_PATH)] = {0};
        const char* iguanaFileobject = objectsCompiledExternaly->expandable[idx];

        snprintf(sourceFilename, MAX_FILENAME_LENGTH + sizeof(TEMP_PATH), "%s%s.c", TEMP_PATH, iguanaFileobject);
    
        int rmStatus = remove(sourceFilename);

        if (rmStatus != 0) 
        {
            Log_w(TAG, "Failed to remove source C file %d", rmStatus);
            return ERROR;
        }
    }        

    free(full_command);

    return SUCCESS;
}
