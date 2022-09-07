/**
 * @file compiler.c
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
#include "../separator/separator.h"
#include "../vector/vector.h"

////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "COMPILER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION


/**
 * @brief Public method for compiling Iguana code into binary
 * 
 * @param codeString[in] pointer to code string buffer
 * @param length[in] length of code string buffer
 * @return Success state
 */
bool Compiler_compile(const char* codeString, const size_t length)
{
    Vector_t tokensVector;

    if(!Separator_getSeparatedWords(codeString, length, &tokensVector))
    {
        Log_e(TAG, "Seperator failed to parse: %s", codeString);
        return ERROR;
    }
    Vector_fit(&tokensVector);
    
    Vector_print(&tokensVector);

    if(!Vector_destroy(&tokensVector))
    {
        Log_e(TAG, "Failed to destroy vector");
        return ERROR;
    }
    
    return SUCCESS;
}