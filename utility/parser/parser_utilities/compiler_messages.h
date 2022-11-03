/**
 * @file compiler_messages.h
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


#ifndef UTILITY_PARSER_PARSER_UTILITIES_COMPILER_MESSAGES_H_
#define UTILITY_PARSER_PARSER_UTILITIES_COMPILER_MESSAGES_H_

#include "../../tokenizer/token/token.h"
#include "stdint.h"

#define NO_ERROR         0

void Shouter_shoutError(const TokenHandler_t tokenHandle, const char* errorMessage);
void Shouter_shoutExpectedToken(const TokenHandler_t tokenHandle, const TokenType_t tokenTypeExpected);
void Shouter_shoutForgottenToken(const TokenHandler_t tokenHandle,const TokenType_t forgottenToken);
void Shouter_shoutUnrecognizedToken(const TokenHandler_t tokenHandle);
void Shouter_resetErrorCount();
uint32_t Shouter_getErrorCount();

#endif // UTILITY_PARSER_PARSER_UTILITIES_COMPILER_MESSAGES_H_