/**
 * @file logger.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-05
 */


#include "logger.h"
#include "../global_config/global_config.h"
#include "colors.h"

////////////////////////////////
// DEFINES

#define COLOR_PRINT_MACRO(COLOR)                \
    va_list args;                               \
    va_start(args, expression);                 \
    logData_(TAG, COLOR, expression, args);     \
    va_end(args);

////////////////////////////////
// PRIVATE CONSTANTS

static const char* TAG = "LOGGER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS

static inline void logData_(const char* TAG, const char* colorString, const char* expression, va_list args);


////////////////////////////////
// IMPLEMENTATION


void Log_d(const char* TAG, const char* expression, ...)
{
    #if VERBOSE_LEVEL > 2
        COLOR_PRINT_MACRO(LIGHT_WHITE);
    #endif
}

void Log_e(const char* TAG, const char* expression, ...)
{
    #if VERBOSE_LEVEL > 0
        COLOR_PRINT_MACRO(LIGHT_RED);
    #endif
}

void Log_i(const char* TAG, const char* expression, ...)
{
    #if VERBOSE_LEVEL > 2
        COLOR_PRINT_MACRO(LIGHT_BLUE);
    #endif
}

void Log_w(const char* TAG, const char* expression, ...)
{
    #if VERBOSE_LEVEL > 1
        COLOR_PRINT_MACRO(YELLOW);
    #endif
}

static inline void logData_(const char* TAG, const char* colorString, const char* expression, va_list args)
{
    printf("%s(%s)-> ", colorString, TAG);
    vprintf(expression, args);
    printf("%s\n", END);
}