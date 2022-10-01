/**
 * @file tokenizer.h
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

#ifndef UTILITY_TOKENIZER_TOKENIZER_H
#define UTILITY_TOKENIZER_TOKENIZER_H

#include <stdlib.h>
#include <stdbool.h>
#include "token/token.h"

TokenHandler_t Tokenizer_wordToCorrespondingToken(const char* seperation, const size_t length);


#endif