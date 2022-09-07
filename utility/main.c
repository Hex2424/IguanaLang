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
    FILE* igFile;
    char buffer[1024];

    igFile = fopen("../test.ig", "r");

    fseek(igFile, 0L, SEEK_END);
    int sz = ftell(igFile);

    fseek(igFile, 0L, SEEK_SET);
    fread(buffer, 1, sizeof(buffer), igFile);
    printf("%d", sz);

    // const char word[] = "int:32 main() \n {};;";

    return Compiler_compile(buffer, sz);
}