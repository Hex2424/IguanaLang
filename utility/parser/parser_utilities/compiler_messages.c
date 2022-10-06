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

////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS


////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

inline void Shouter_shoutError(const TokenHandler_t tokenHandle, const char* errorMessage)
{

    Logc_e("%s:%u:%u -> %s",tokenHandle->location.filename,tokenHandle->location.line, tokenHandle->location.column, errorMessage);
}

inline void Shouter_shoutExpectedToken(const TokenHandler_t tokenHandle,const TokenType_t tokenTypeExpected)
{

    Logc_e("%s:%u:%u -> %s \'%d\'",tokenHandle->location.filename,tokenHandle->location.line, tokenHandle->location.column, tokenTypeExpected);
}
