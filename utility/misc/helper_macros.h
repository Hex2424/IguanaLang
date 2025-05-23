/**
 * @file helper_macros.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2025-04-22
 */

#ifndef UTILITY_MISC_HELPER_MACROS_H_
#define UTILITY_MISC_HELPER_MACROS_H_


// STRING HELPERS

#define STRINGIFY(exp) str__(exp)
#define str__(exp) #exp

#define SIZEOF_NOTERM(string) (sizeof(string) - 1)



#endif // UTILITY_MISC_HELPER_MACROS_H_