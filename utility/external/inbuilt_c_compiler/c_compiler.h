/**
 * @file c_compiler.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-21
 */
#ifndef UTILITY_EXTERNAL_INBUILT_C_COMPILER_C_COMPILER_H_
#define UTILITY_EXTERNAL_INBUILT_C_COMPILER_C_COMPILER_H_
#include "../../parser/structures/import_object/import_object.h"
#include <stdbool.h>
#include "../../vector/vector.h"

bool CExternalCompiler_compile(const char* objectId);
bool CExternalCompiler_compileWhole(const VectorHandler_t pathsOfCompiledIguana);

#endif // UTILITY_EXTERNAL_INBUILT_C_COMPILER_C_COMPILER_H_