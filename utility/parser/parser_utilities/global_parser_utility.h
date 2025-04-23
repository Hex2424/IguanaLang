/**
 * @file global_parser_utility.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-09
 */

#ifndef UTILITY_PARSER_PARSER_UTILITIES_GLOBAL_PARSER_UTILITY_H_
#define UTILITY_PARSER_PARSER_UTILITIES_GLOBAL_PARSER_UTILITY_H_

#include "../../tokenizer/token/token.h"
#include "compiler_messages.h"
#include "../structures/pattern/pattern.h"
#include "stdbool.h"


bool ParserUtils_skipUntil(TokenHandler_t** currentTokenHandle, const TokenType_t untilTokenType);
bool ParserUtils_tryParseSequence(TokenHandler_t** currentTokenHandle, const TokenType_t* pattern,const size_t patternSize);
bool ParserUtils_assignTokenValue(char** to, const char* from);

#endif // UTILITY_PARSER_PARSER_UTILITIES_GLOBAL_PARSER_UTILITY_H_