/**
 * @file compiler.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-07
 */
#ifndef UTILITY_COMPILER_COMPILER_H_
#define UTILITY_COMPILER_COMPILER_H_

#include <stdlib.h>
#include <stdbool.h>

bool Compiler_compile(const char* codeString, const size_t length);


#endif // UTILITY_COMPILER_COMPILER_H_