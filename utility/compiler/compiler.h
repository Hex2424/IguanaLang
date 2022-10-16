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
#include "../vector/vector.h"

typedef struct
{
    Vector_t alreadyCompiledFilePaths;
    Queue_t filePathsToCompile;

}Compiler_t;

typedef Compiler_t* CompilerHandle_t;


bool Compiler_initialize(CompilerHandle_t compiler);
bool Compiler_destroy(CompilerHandle_t compiler);
bool Compiler_startCompilingProcessOnRoot(CompilerHandle_t compiler, const char* filePath);

#endif // UTILITY_COMPILER_COMPILER_H_