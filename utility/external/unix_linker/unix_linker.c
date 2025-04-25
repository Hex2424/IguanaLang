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
#include <safety_macros.h>
#include <global_config.h>


////////////////////////////////
// DEFINES
#define LD_LINKER_COMMAND "ld -static -o output --entry " MAIN_PROCESS_FILE_NAME " -lc"

////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "UNIX_LINKER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool UnixLinker_linkPaths(const VectorHandler_t objectsCompiledExternaly)
{
    char* full_command;
    char* iterator;

    ALLOC_CHECK(full_command, sizeof(LD_LINKER_COMMAND) + CFILES_LENGTH + (CFILES_LENGTH * objectsCompiledExternaly->currentSize), ERROR);

    full_command[0] = '\0';//null terminator beggining for strncat
    iterator = full_command;

    strcat(iterator, LD_LINKER_COMMAND);
    iterator += (sizeof(LD_LINKER_COMMAND) - 1);

    for(size_t idx = 0; idx < objectsCompiledExternaly->currentSize; idx++)
    {
        const char* iguanaFileobject = objectsCompiledExternaly->expandable[idx];

        NULL_GUARD(iguanaFileobject, ERROR, Log_e(TAG, "Null object in linker passed"));

        snprintf(iterator, CFILES_LENGTH + sizeof(' '), " %s%s.o", TEMP_PATH, iguanaFileobject);
        iterator += (CFILES_LENGTH);
    }

    Log_d(TAG, "Executing Linker: %s", full_command);

    if(system(full_command) == -1)
    {
        free(full_command);
        return ERROR;
    }


    Log_i(TAG, "Linked objects successfuly!");
    free(full_command);

    return SUCCESS;
}