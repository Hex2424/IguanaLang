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

// TODO: do better platform handling for each architecture
#if defined(TARGET_RENESAS_RL78)
    #define ARCHITECTURE_DEFAULT_BITS       16
#elif defined(TARGET_RENESAS_RX) || defined(TARGET_RENESAS_RA) || defined(TARGET_RENESAS_SUPERH)
    #define ARCHITECTURE_DEFAULT_BITS       32
#elif defined(TARGET_RENESAS_RZ) && defined(__aarch64__)
    #define ARCHITECTURE_DEFAULT_BITS       64
#else
    #define ARCHITECTURE_DEFAULT_BITS       64
#endif

// for now lets leave 64 bit default


#endif // UTILITY_MISC_ARCH_SPECIFIC_H_