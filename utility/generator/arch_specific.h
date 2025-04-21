/**
 * @file arch_specific.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2025-04-21
 */

#ifndef UTILITY_MISC_ARCH_SPECIFIC_H_
#define UTILITY_MISC_ARCH_SPECIFIC_H_

#include <stdbool.h>
#include <stdint.h>
#include "csyntax_database.h"
#include "config_generator.h"

// TODO: do better platform handling for each architecture
// #if defined(TARGET_RENESAS_RL78)
//     static const char* BITPACK_TYPE = "typedef uint16_t bitpack_t";
// #elif defined(TARGET_RENESAS_RX) || defined(TARGET_RENESAS_RA) || defined(TARGET_RENESAS_SUPERH)
//     static const char* BITPACK_TYPE = "typedef uint32_t bitpack_t";
// #elif defined(TARGET_RENESAS_RZ) && defined(__aarch64__)
//     static const char* BITPACK_TYPE = "typedef uint64_t bitpack_t";
// #else
//     static const char* BITPACK_TYPE = "typedef uintptr_t bitpack_t";
// #endif

// for now lets leave 64 bit default

#define BIT_TYPE_DEF              TYPE_BIT64_DEF
#define BIT_SIZE_BITPACK          64
#define PARAM_TYPE_DEF            TYPE_BIT0_DEF POINTER_SIGN_DEF


#define BITPACK_TYPE_TYPEDEF_DEF  TYPEDEF_WRAP(BIT_TYPE_DEF, BITPACK_TYPE_NAME)


#endif // UTILITY_MISC_ARCH_SPECIFIC_H_