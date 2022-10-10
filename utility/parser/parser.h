/**
 * @file parser.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-07
 */

#ifndef UTILITY_PARSER_PARSER_H_
#define UTILITY_PARSER_PARSER_H_

#include "../vector/vector.h"
#include "../logger/logger.h"
#include "structures/main_frame/main_frame.h"
#include "stdbool.h"
#include "../tokenizer/token/token.h"

bool Parser_initialize();
bool Parser_parseTokens(MainFrameHandle_t root, const VectorHandler_t tokenVector);
bool Parser_destroy();
#endif // UTILITY_PARSER_PARSER_H_