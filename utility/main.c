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
#include "logger/logger.h"
#include "separator/separator.h"

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
    const char word[] = "int:32 main() \n {};;";
    if(!Separator_getSeparatedWords(word, sizeof(word)))
    {
        Log_e(TAG, "Seperator failed to parse: %s", word);
        return -1;
    }
    
    return 0;
}