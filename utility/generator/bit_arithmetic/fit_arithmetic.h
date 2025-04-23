/**
 * @file fit_arithmetic.h
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

#ifndef UTILITY_MISC_FIT_ARITHMETIC_H_
#define UTILITY_MISC_FIT_ARITHMETIC_H_

#include <stdbool.h>
#include <stdint.h>

#include "../csyntax_database.h"
#include "../config_generator.h"
#include <arch_specific.h>

// ALL THESE ARITHMETICS WORKS IF BIT WIDTH < BIT_TYPE_EXP
// Otherwise other arithmetic should be used


#define AFIT_MASK(bit_count) ((((BIT_TYPE_EXP) 0x1) << bit_count) - 1)

// VALUE RESET
#define AFIT_RESET(platform_var, bit_pos, bit_count) (platform_var & (~(AFIT_MASK(bit_count) << (BIT_SIZE_BITPACK - (bit_pos + bit_count)))))

// VALUE READING
#define AFIT_READ(platform_var, bit_pos, bit_count) (((BIT_TYPE_EXP) platform_var) >> (BIT_SIZE_BITPACK - (bit_pos + bit_count)))

// VALUE SET
#define AFIT_SET_NORMALIZED_VAR(src_shift_var, dest_var, bit_pos_dest, bit_count) (dest_var = (AFIT_RESET(dest_var, bit_pos_dest, bit_count) | ((src_shift_var & AFIT_MASK(bit_count)) << (BIT_SIZE_BITPACK - (bit_pos_dest + bit_count)))))

#define AFIT_SET_CONST(shifted_const, bit_pos, bit_count) (((BIT_TYPE_EXP) platform_var) >> (BIT_SIZE_BITPACK - (bitpos + bit_count)))

#endif // UTILITY_MISC_FIT_ARITHMETIC_H_