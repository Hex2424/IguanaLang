/**
 * @file c_compiler_macros.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-12-03
 */
#ifndef UTILITY_EXTERNAL_INBUILT_C_COMPILER_C_COMPILER_MACROS_H_
#define UTILITY_EXTERNAL_INBUILT_C_COMPILER_C_COMPILER_MACROS_H_
#include "../../global_config/global_config.h"

#define ADD_WARNING(warningID) "-Wno-" warningID" "



#if VERBOSE_C_COMPILER
    #define LOG_PATH_TO_FILE ""
#else
    #define LOG_PATH_TO_FILE " 2> clog.err"
#endif


#if C_COMPILER_WARNING_SUPRESSION
    #define WARNING_SUPRESSED ""
#else
    #define WARNING_SUPRESSED                   \
    ADD_WARNING("overflow")                     \
    ADD_WARNING("implicit-function-declaration")
#endif


#endif // UTILITY_EXTERNAL_INBUILT_C_COMPILER_C_COMPILER_MACROS_H_