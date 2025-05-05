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

#include <stdbool.h>
#include <vector.h>

// bool CExternalCompiler_compile(const char* filename, const bool removeSourceAfter);
// bool CExternalCompiler_link(const VectorHandler_t objectsCompiledExternaly, char* outputName);

bool CExternalCompiler_compile(const VectorHandler_t objectsCompiledExternaly, char* outputName, const bool linkingEnabled);

#endif // UTILITY_EXTERNAL_INBUILT_C_COMPILER_C_COMPILER_H_