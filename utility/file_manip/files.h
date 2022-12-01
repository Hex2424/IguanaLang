/**
 * @file files.h
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
#ifndef UTILITY_FILEMANIP_FILES_H_
#define UTILITY_FILEMANIP_FILES_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


char* Files_filepathExtensionPointer(const char* filePath);
uint16_t Files_filepathGetFilename(const char* filePath, char** fileName);

#endif // UTILITY_FILEMANIP_FILES_H_