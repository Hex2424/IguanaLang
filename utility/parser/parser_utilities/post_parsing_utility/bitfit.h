/**
 * @file bitfit.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2025-04-23
 */

#ifndef UTILITY_PARSER_PARSER_UTILITIES_BITFIT_H_
#define UTILITY_PARSER_PARSER_UTILITIES_BITFIT_H_

#include <stdbool.h>
#include <stdint.h>
#include <hashmap.h>
#include <typedefs.h>
#include <vector.h>

typedef enum
{
    FIRST_FIT,
    NEXT_FIT,
    FULL_FIT
}BitFitMethod_t;

bool Bitfit_assignGroupsAndPositionForVariableHashmap_(const HashmapHandle_t variablesHashmap, const BitFitMethod_t fitType, BitpackSize_t* sizeNeededForVariables);
bool Bitfit_assignGroupsAndPositionForVariableVector_(const VectorHandler_t variablesVector, const BitFitMethod_t fitType, BitpackSize_t* sizeNeededForVariables);

#endif // UTILITY_PARSER_PARSER_UTILITIES_BITFIT_H_