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
#include <unix_linker.h>
#include <c_compiler.h>
#include <global_config.h>
#include <libgen.h>

bool Compiler_compileIguana(const char* iguanaFilePath);
bool Compiler_initialize(const char* mainFilePath);

void Compiler_removeExtensionFromFilenameWithCopy_(char* filename, const char* const filenameWithExtension);
#endif // UTILITY_COMPILER_COMPILER_H_