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


////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool Generator_generateCodeFor(const char* relativeIguanaFilePath, const MainFrameHandle_t ast)
{
    FILE* cFile;
    char* cFilename;
    int iguanaPathLength;

    iguanaPathLength = strlen(relativeIguanaFilePath);
    
    cFilename = strrchr(relativeIguanaFilePath, '/');    // finding filename start
    // cFilename[iguanaPathLength - 2] = '.';               // /ssss.c
    cFilename[iguanaPathLength - 1] = 'c';

    cFile = fopen(cFilename, "r");

}


bool getIguanaFileName_()
{

}