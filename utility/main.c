/**
 * @file main.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-04
 */
#include "compiler/compiler.h"
#include <stdio.h>
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "MAIN";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION




int main(int argc, char const *argv[])
{
    // if(argc < 2)
    // {
    //     printf("Need provide files for compilation\n");
    //     return 0;
    // }

    return Compiler_compile("../test.ig");
}