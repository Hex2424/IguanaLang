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

#include "separator/separator.h"

int main(int argc, char const *argv[])
{
    const char word[] = "int:32 main() {}";
    Separator_getSeparatedWords(word, sizeof(word));
    
    return 0;
}