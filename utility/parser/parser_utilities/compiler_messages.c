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

void Shouter_shoutError(const TokenHandler_t tokenHandle, const char* errorMessage)
{

    Logc_e("%s:%u:%u -> %s",
    tokenHandle->location.filename,
    tokenHandle->location.line,
    tokenHandle->location.column,

    errorMessage);
}

void Shouter_shoutExpectedToken(const TokenHandler_t tokenHandle,const TokenType_t tokenTypeExpected)
{

    Logc_e("%s:%u:%u -> %s \'%s\', found this '%s' -_-",
    tokenHandle->location.filename,
    tokenHandle->location.line,
    tokenHandle->location.column,
    
    EXPECTED_TOKEN, 
    bindingsTable_[tokenTypeExpected].expression,
    tokenHandle->valueString);
}


void Shouter_shoutUnrecognizedToken(const TokenHandler_t tokenHandle)
{

    Logc_e("%s:%u:%u -> Unrecognized token \'%s\'",
    tokenHandle->location.filename,
    tokenHandle->location.line,
    tokenHandle->location.column,

    tokenHandle->valueString);
}


void Shouter_shoutForgottenToken(const TokenHandler_t tokenHandle,const TokenType_t forgottenToken)
{

    Logc_e("%s:%u:%u -> Forgotten token: \'%s\'",
    tokenHandle->location.filename,
    tokenHandle->location.line,
    tokenHandle->location.column,
    
    bindingsTable_[forgottenToken].expression,
    tokenHandle->tokenType);
}

