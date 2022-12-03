/**
 * @file files.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-12-01
 */

#include "files.h"
#include <string.h>
#include "../misc/safety_macros.h"
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS


////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

/**
 * @brief Public method to get filename and by full Filepath (not parental)
 * 
 * @return FilenameLength
 */

/**
 * @brief Public method to get filename and by full Filepath (not parental)
 * 
 * @param[in] filePath  - Real file path
 * @param[out] fileName - Pointer to Pointer of filename start position
 * @return uint16_t     - Filename length
 */
uint16_t Files_filepathGetFilename(const char* filePath, char** fileName)
{
    *fileName = strrchr(filePath, '/');
    if(*fileName == NULL) // incase of slash not found
    {
        *fileName = filePath;
    }else
    {
        (*fileName)++;
    }
    
    char* extensionPointer = Files_filepathExtensionPointer(*fileName);
    if(extensionPointer == NULL)
    {
        return strlen(*fileName);
    }

    return ((extensionPointer- SIZE_STR(".")) - (*fileName));
}

/**
 * @brief Public method to get Extension pointer out of filePath
 * 
 * @param[in] filePath - Filepath or filename with extension on the end
 * @return char*       - returns pointer to extension first element if no extension returns NULL
 */
char* Files_filepathExtensionPointer(const char* filePath)
{
    char* extensionPointer;
    extensionPointer = strrchr(filePath, '.');
    if(extensionPointer == NULL)
    {
        return NULL;
    }else
    {
        return extensionPointer + SIZE_STR(".");
    }
    

}