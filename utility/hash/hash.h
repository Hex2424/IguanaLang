/**
 * @file hash.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-16
 */

#ifndef HASH_HASH_H_
#define HASH_HASH_H_

#include "stdbool.h"
#include "stdint.h"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

bool EHash_hash(const char* text, const uint32_t inputSize, char* output, const uint32_t outputSize);

#endif // HASH_HASH_H_
