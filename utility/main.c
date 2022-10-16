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

static const char* TAG = "MAIN";

// inline int variable = 0;

// struct ssss
// {
//     /* data */
// };

int main(int argc, char const *argv[])
{
    Compiler_t compiler;
    char filePathToCompile[] = "../iguanaTestFiles/test.i";
    // if(argc < 2)
    // {
    //     printf("Need provide files for compilation\n");
    //     return 0;
    // }
    if(!Compiler_initialize(&compiler))
    {
        return -1;
    }

    return Compiler_startCompilingProcessOnRoot(&compiler, filePathToCompile);
}