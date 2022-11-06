/**
 * @file compiler_messages.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-06
 */

#include "compiler_messages.h"
#include "../../tokenizer/token/token_database/token_bindings.h"
#include <stdarg.h>
#include "../../logger/colors.h"
////////////////////////////////
// DEFINES

#define SHOUT_MESSAGE(COLOR)                    \
    if(tokenHandle != NULL)                     \
    printLocation_(tokenHandle);                \
        va_list args;                           \       
        va_start(args, errorMessage);           \         
        Logcc(errorMessage, COLOR, args);   \
        va_end(args);                           \

////////////////////////////////
// PRIVATE CONSTANTS


////////////////////////////////
// PRIVATE TYPES
uint32_t errorCount = NO_ERROR;

////////////////////////////////
// PRIVATE METHODS
static void incrementErrorCount_();
static void printLocation_(const TokenHandler_t tokenHandle);
////////////////////////////////
// IMPLEMENTATION

void Shouter_shoutError(const TokenHandler_t tokenHandle, const char* errorMessage, ...)
{
    SHOUT_MESSAGE(LIGHT_RED);
    incrementErrorCount_();
}

void Shouter_shoutWarning(const TokenHandler_t tokenHandle, const char* errorMessage, ...)
{
    SHOUT_MESSAGE(YELLOW);
}

void Shouter_shoutInfo(const TokenHandler_t tokenHandle, const char* errorMessage, ...)
{
    SHOUT_MESSAGE(LIGHT_BLUE);
}

static void printLocation_(const TokenHandler_t tokenHandle)
{
    printf("%s:%u:%u -> ",
    tokenHandle->location.filename,
    tokenHandle->location.line,
    tokenHandle->location.column);
}

void Shouter_shoutExpectedToken(const TokenHandler_t tokenHandle,const TokenType_t tokenTypeExpected)
{
    printLocation_(tokenHandle);
    Logc("Expected token \'%s\', found this '%s' -_-",
    LIGHT_RED,
    bindingsTable_[tokenTypeExpected].expression,
    tokenHandle->valueString);
    incrementErrorCount_();
}


void Shouter_shoutUnrecognizedToken(const TokenHandler_t tokenHandle)
{
    printLocation_(tokenHandle);
    Logc("Unrecognized token \'%s\'",
    LIGHT_RED,
    tokenHandle->valueString);
    incrementErrorCount_();
}


void Shouter_shoutForgottenToken(const TokenHandler_t tokenHandle,const TokenType_t forgottenToken)
{
    printLocation_(tokenHandle);
    Logc("Forgotten token: \'%s\'",
    bindingsTable_[forgottenToken].expression,
    tokenHandle->tokenType);

    incrementErrorCount_();
}

inline void Shouter_resetErrorCount()
{
    errorCount = 0;
}

inline uint32_t Shouter_getErrorCount()
{
    return errorCount;
}

static inline void incrementErrorCount_()
{
    errorCount++;
}