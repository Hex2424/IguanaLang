/**
 * @file safety_macros.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-06
 */

#ifndef UTILITY_MISC_SAFETY_MACROS_H_
#define UTILITY_MISC_SAFETY_MACROS_H_

#include "stdbool.h"

#define SUCCESS                 true
#define ERROR                   false




#define NULL_GUARD(variable, returning, print)  \
    if(variable == NULL)                        \
    {                                           \
        print;                                  \
        return returning;                       \
    }                                       

#define ALLOC_CHECK(variable,size, returning)      \
    variable = malloc(size);               \
    NULL_GUARD(variable, returning,                         \
    Log_e(TAG, "Memory cannot be allocated, heap issue"))


#define REALLOC_CHECK(variable, size, returning)                \
    variable = realloc(variable, size);                       \
    NULL_GUARD(variable, ERROR, Log_e(TAG, "Realloc failed for some reason, heap issue"))


/**
 * @brief Macro for returning String length with excluded null terminator which has length of 1
 * 
 */
#define SIZE_STR(string) (sizeof(string) - 1)

#endif // UTILITY_MISC_SAFETY_MACROS_H_