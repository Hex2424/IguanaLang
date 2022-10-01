/**
 * @file token.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana+ and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-05
 */

#ifndef UTILITY_TOKENIZER_TOKEN_TOKEN_H
#define UTILITY_TOKENIZER_TOKEN_TOKEN_H

// #include <stdbool.h>
// #include <stdint.h>
// #include <stdlib.h>

#include "token_database/token_types.h"

typedef struct
{
    TokenType_t tokenType;
    char* valueString;
}Token_t;

typedef Token_t* TokenHandler_t;


#endif