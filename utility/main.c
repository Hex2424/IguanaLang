/**
 * @file main.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-04
 */

#include "compiler/compiler.h"
#include <stdio.h>
#include <string.h>

static const char* TAG = "MAIN";
static const char* igRelativeTempPath = "../iguanaTestFiles/test.i";

#define SEPERATOR_FIXER "/"
// inline int variable = 0;

// struct ssss
// {
//     /* data */
// };

int main(int argc, char const *argv[])
{
    Compiler_t compiler;
    uint32_t sizeExePath, sizeIguanaFilePath;
    char* filePathToCompile;
    sizeExePath = strlen(argv[0]);
    for(uint32_t i = sizeExePath; i > 0; i--)
    {
        if(argv[0][i] == '/')
        {
            break;
        }
        sizeExePath--;
    }

    sizeIguanaFilePath = strlen(igRelativeTempPath);
    filePathToCompile = malloc(sizeExePath + sizeIguanaFilePath + sizeof(SEPERATOR_FIXER));
    strncat(filePathToCompile, argv[0], sizeExePath);
    strcat(filePathToCompile, SEPERATOR_FIXER);
    strcat(filePathToCompile, igRelativeTempPath);

    // if(argc < 2)
    // {
    //     printf("Need provide files for compilation\n");
    //     return 0;
    // }
    if(!Compiler_initialize(&compiler))
    {
        return -1;
    }

    if(!Compiler_startCompilingProcessOnRoot(&compiler, filePathToCompile))
    {
        return -2;
    }

    return Compiler_destroy(&compiler);
}