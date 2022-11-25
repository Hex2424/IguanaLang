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
#include "../../global_config/global_config.h"
#include "string.h"
#include "stdio.h"
#include "../../misc/safety_macros.h"
#include "../../logger/logger.h"

////////////////////////////////
// DEFINES
#define GCC_COMPILER_COMMAND "gcc -c %s%s.c -o %s.o 2> clog.err"

////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "C_COMPILER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool CExternalCompiler_compile(const char* objectId)
{
    char full_command[sizeof(GCC_COMPILER_COMMAND) + CFILES_LENGTH] = GCC_COMPILER_COMMAND;
    char check_command[CFILES_LENGTH];
    snprintf(check_command, CFILES_LENGTH, "%s%s.o", TEMP_PATH, objectId);
    snprintf(full_command, sizeof(GCC_COMPILER_COMMAND) + CFILES_LENGTH, GCC_COMPILER_COMMAND, TEMP_PATH, objectId, objectId);
    Log_d(TAG, "Executing command:%s", full_command);

    if(system(full_command) == -1)
    {
        return ERROR;
    }

    FILE* objectFile = fopen(check_command, "r");

    NULL_GUARD(objectFile, ERROR, Log_e(TAG, "Failed to compile one of c files"));

    if(fclose(objectFile) != 0)
    {
        Log_w(TAG, "Failed to close file at runtime: %s", objectFile);
        return ERROR;
    }

    return SUCCESS;
}

bool CExternalCompiler_compileWhole(const VectorHandler_t pathsOfCompiledIguana)
{
    size_t idx;
    for(idx = 0; idx < pathsOfCompiledIguana->currentSize; idx++)
    {
        ImportObjectHandle_t import;
        import = pathsOfCompiledIguana->expandable[idx];

        NULL_GUARD(import, ERROR, Log_e(TAG, "import is null"));
        CExternalCompiler_compile(import->objectId.id);
    }

    return SUCCESS;
}