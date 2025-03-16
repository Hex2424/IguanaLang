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


#define GCC_COMPILER_COMMAND "gcc -c %s%s.c -o %s.o "WARNING_SUPRESSED LOG_PATH_TO_FILE

////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "C_COMPILER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool CExternalCompiler_compile(const char* filename)
{
    int closeStatus = true;

    char full_command[sizeof(GCC_COMPILER_COMMAND) + CFILES_LENGTH] = GCC_COMPILER_COMMAND;
    char check_command[CFILES_LENGTH];
    snprintf(check_command, CFILES_LENGTH, "%s%s.o", TEMP_PATH, filename);
    snprintf(full_command, sizeof(GCC_COMPILER_COMMAND) + CFILES_LENGTH, GCC_COMPILER_COMMAND, TEMP_PATH, filename, filename);
    Log_d(TAG, "Executing command:%s", full_command);

    if(system(full_command) == -1)
    {
        return ERROR;
    }

    FILE* objectFile = fopen(check_command, "r");

    NULL_GUARD(objectFile, ERROR, Log_e(TAG, "Failed to compile one of c files"));

    closeStatus = fclose(objectFile);
    
    if(closeStatus != 0)
    {
        Log_w(TAG, "Failed to close C file at runtime: %d", closeStatus);
        return ERROR;
    }

    return SUCCESS;
}