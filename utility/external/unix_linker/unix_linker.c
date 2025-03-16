/**
 * @file unix_linker.c
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
#include "unix_linker.h"
#include "string.h"


////////////////////////////////
// DEFINES
#define LD_LINKER_COMMAND "./ld/ld_linker -dynamic-linker ./ld/ld-linux-x86-64.so -o output --entry ___start -lc"

////////////////////////////////
// PRIVATE CONSTANTS
// static const char* TAG = "UNIX_LINKER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

// bool UnixLinker_linkPaths(const VectorHandler_t objectsCompiledExternaly)
// {
//     char* full_command;
//     char* iterator;

//     ALLOC_CHECK(full_command, sizeof(LD_LINKER_COMMAND) + CFILES_LENGTH + (OBJECT_ID_LENGTH * objectsCompiledExternaly->currentSize), ERROR);

//     full_command[0] = '\0';//null terminator beggining for strncat
//     iterator = full_command;

//     strncat(iterator, LD_LINKER_COMMAND,sizeof(LD_LINKER_COMMAND));
//     iterator += (sizeof(LD_LINKER_COMMAND) - 1);

//     for(size_t idx = 0; idx < objectsCompiledExternaly->currentSize; idx++)
//     {
//         ImportObjectHandle_t object;
//         object = objectsCompiledExternaly->expandable[idx];

//         NULL_GUARD(object, ERROR, Log_e(TAG, "Null object in linker passed"));

//         snprintf(iterator, CFILES_LENGTH + sizeof(' '), " %s%s.o", TEMP_PATH, object->objectId.id);
//         iterator += (CFILES_LENGTH);
//     }
//     // char check_command[CFILES_LENGTH];


//     // snprintf(check_command, CFILES_LENGTH, "%s%s.o", TEMP_PATH, objectId);
//     // snprintf(full_command, sizeof(GCC_COMPILER_COMMAND) + CFILES_LENGTH, GCC_COMPILER_COMMAND, TEMP_PATH, objectId, objectId);
//     // Log_d(TAG, "Executing command:%s", full_command);

//     if(system(full_command) == -1)
//     {
//         free(full_command);
//         return ERROR;
//     }

//     // FILE* objectFile = fopen(check_command, "r");

//     // NULL_GUARD(objectFile, ERROR, Log_e(TAG, "Failed to compile one of c files"));

//     // if(fclose(objectFile) != 0)
//     // {
//     //     Log_w(TAG, "Failed to close file at runtime: %s", objectFile);
//     //     return ERROR;
//     // }
//     free(full_command);

//     return SUCCESS;
// }