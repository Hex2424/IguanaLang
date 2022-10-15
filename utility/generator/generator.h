/**
 * @file generator.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-11
 */

#ifndef UTILITY_GENERATOR_GENERATOR_H_
#define UTILITY_GENERATOR_GENERATOR_H_

#include "stdint.h"
#include "stdbool.h"
#include "../parser/structures/main_frame/main_frame.h"
#include <stdio.h>

#define FOUT_BUFFER_LENGTH          1024

typedef struct
{
    char* relativeIguanaFilePath;
    char writingbuffer[FOUT_BUFFER_LENGTH];
    FILE* hFile;
    FILE* cFile;
    MainFrameHandle_t ast;
} CodeGenerator_t;

typedef CodeGenerator_t* CodeGeneratorHandle_t;

bool Generator_initialize(CodeGeneratorHandle_t generator, const char* relativeIguanaFilePath, const MainFrameHandle_t ast);
bool Generator_generateCode(CodeGeneratorHandle_t generator);

#endif // UTILITY_GENERATOR_GENERATOR_H_