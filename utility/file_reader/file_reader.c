/**
 * @file file_reader.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-10
 */
#include "file_reader.h"
#include <stdio.h>
#include "../logger/logger.h"
#include "../misc/safety_macros.h"
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "FILE_READER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION


/**
 * @brief Public method used to read Iguana file to corresponding size buffer
 * 
 * @param fileName[in] Input of Iguana file path
 * @param buffer[out] output dynamically allocated buffer with file readed into it
 * @return size_t[out] returning on error -1, on success size of buffer
 */
size_t FileReader_readToBuffer(const char* fileName, char** buffer)
{
    FILE* igFile;
    size_t fileSize;

    igFile = fopen(fileName, "r");

    fseek(igFile, 0L, SEEK_END);
    fileSize = ftell(igFile);

    Log_i(TAG, "Iguana file: %s size: %d", fileName, fileSize);
    *buffer = malloc(fileSize);
    
    ALLOC_CHECK(*buffer, -1);

    fseek(igFile, 0L, SEEK_SET);

    fread(*buffer, 1, fileSize, igFile);
    fclose(igFile);
    return fileSize;
}
