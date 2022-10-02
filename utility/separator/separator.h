/**
 * @file separator.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-05
 */

#ifndef UTILITY_SEPERATOR_SEPARATOR_H
#define UTILITY_SEPERATOR_SEPARATOR_H
#include "../vector/vector.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool Separator_getSeparatedWords(const char* codeString, const size_t length, VectorHandler_t vector);




#endif