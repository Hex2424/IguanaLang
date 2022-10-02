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

#define SUCCESS                 true
#define ERROR                   false

#define NULL_GUARD(variable, returning, print)  \
    if(variable == NULL)                        \
    {                                           \
        print;                                  \
        return returning;                       \
    }                                       

#define ALLOC_CHECK(variable, returning)                \
    NULL_GUARD(variable, returning,                         \
    Log_e(TAG, "Memory cannot be allocated, heap issue")    \
)



#endif // UTILITY_MISC_SAFETY_MACROS_H_