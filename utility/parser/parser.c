/**
 * @file parser.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-07
 */
#include "parser.h"

////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS


////////////////////////////////
// PRIVATE TYPES
static const char* TAG = "PARSER";

////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool Parser_parseTokens(VectorHandler_t tokenList)
{
    MainFrame_t root;

    if(!MainFrame_init(&root))
    {
        Log_e(TAG, "MainFrame init failed");
        return ERROR;
    }

    

    return SUCCESS;
}
