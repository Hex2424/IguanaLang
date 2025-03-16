/**
 * @file compiler.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
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
#include "../queue/queue.h"
#include <vector.h>
#include <hashmap.h>


bool Compiler_compile(const char* iguanaFilePath, const bool isMain);
bool Compiler_initialize(const char* mainFilePath);

#endif // UTILITY_COMPILER_COMPILER_H_