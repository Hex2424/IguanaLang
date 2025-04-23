/**
 * @file plt_arithmetic.h
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

#ifndef UTILITY_MISC_PLT_ARITHMETIC_H_
#define UTILITY_MISC_PLT_ARITHMETIC_H_

#include <stdbool.h>
#include <stdint.h>

#include "../csyntax_database.h"
#include "../config_generator.h"
#include <arch_specific.h>

// ALL THESE ARITHMETICS WORKS IF BIT_WIDTH == BIT_TYPE_EXP
// Otherwise other arithmetic should be used

// VALUE RESET
#define APLT_RESET(platform_var) (platform_var = (BIT_TYPE_EXP) 0)

// VALUE READING
#define APLT_READ(platform_var) ((BIT_TYPE_EXP) platform_var)

// VALUE BINARY OR (|)
#define APLT_OR(src, dest) (((BIT_TYPE_EXP) platform_var) | ((BIT_TYPE_EXP) dest))

// VALUE BINARY AND (&)
#define APLT_AND(src, dest) (((BIT_TYPE_EXP) src) & ((BIT_TYPE_EXP) dest))

// VALUE BINARY XOR (^)
#define APLT_XOR(src, dest) (((BIT_TYPE_EXP) src) ^ ((BIT_TYPE_EXP) dest))

// VALUE BINARY NOT (~)
#define APLT_NOT(src) (~((BIT_TYPE_EXP) src))

// VALUE BINARY SUM (+)
#define APLT_SUM(src, dest) (((BIT_TYPE_EXP) src) + ((BIT_TYPE_EXP) dest))

// VALUE BINARY MINUS (-)
#define APLT_MINUS(src, dest) (((BIT_TYPE_EXP) src) - ((BIT_TYPE_EXP) dest))


// VALUE SET
#define APLT_SET_VAR(src, dest) (dest = src)

#define APLT_SET_CONST(dest, constvar) (dest = ((BIT_TYPE_EXP) constvar))


#endif // UTILITY_MISC_PLT_ARITHMETIC_H_