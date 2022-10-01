/**
 * @file file_reader.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana+ and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-10
 */


#ifndef UTILITY_FILE_READER_FILE_READER_H_
#define UTILITY_FILE_READER_FILE_READER_H_
#include "stdlib.h"


size_t FileReader_readToBuffer(const char* fileName, char** buffer);



#endif // UTILITY_FILE_READER_FILE_READER_H_