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

////////////////////////////////
// DEFINES
#define LINUX_TEMP_FOLDER_PATH      "/tmp"
#define WINDOWS_TEMP_FOLDER_PATH    "ECHO %Temp%" // get from CMD

#if OS == LINUX
    #define TEMP_PATH                   LINUX_TEMP_FOLDER_PATH
#elif OS == WINDOWS
    #define TEMP_PATH                   WINDOWS_TEMP_FOLDER_PATH
#endif
////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "GENERATOR";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS

bool iguanaPathToCharfilePath_(char* relativeIguanaFilePath, const char cFormatExtension);

////////////////////////////////
// IMPLEMENTATION

bool Generator_generateCodeFor(const char* relativeIguanaFilePath, const MainFrameHandle_t ast)
{
    FILE* cFile;
    char* cFilename;
    
    cFilename = strrchr(relativeIguanaFilePath, '/');    // finding filename start
    // cFilename[iguanaPathLength - 2] = '.';               // /ssss.c
    cFilename[iguanaPathLength - 1] = 'c';

    cFile = fopen(cFilename, "r");

}

/**
 * @brief Private method for effective file extension changing which are 1 char length
 * 
 * e.g: ./filepath/iguana.i --> ./filepath/iguana.c
 * e.g: ./filepath/iguana.ig --> ./filepath/iguana.c
 * e.g: ./filepath/iguana.iguana --> ./filepath/iguana.h
 * 
 * @param[out] relativeIguanaFilePath   Relative path from main root folder to iguana file which being compiled
 * @param[in] cFormatExtension          char to replace extension with (must be char and not string)
 * @return                              Succes state 
 */
bool iguanaPathToCharfilePath_(char* relativeIguanaFilePath, const char cFormatExtension)
{
    char* pointerDotStart;

    pointerDotStart = strrchr(relativeIguanaFilePath, '.');
    NULL_GUARD(pointerDotStart, false, Log_e(TAG, "pointer to dot of filename is null"));
    pointerDotStart[1] = cFormatExtension;
    pointerDotStart[2] = '\0';
    return true;
}

