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

#include <vector.h>
#include <logger.h>
#include "structures/main_frame/main_frame.h"
#include <stdbool.h>
#include "../tokenizer/token/token.h"
#include "../compiler/compiler.h"


typedef struct
{
    char* currentFolderPath;
    size_t currentFolderPathLength;
}Parser_t;

typedef Parser_t* ParserHandle_t;

bool Parser_initialize(ParserHandle_t parser);
bool Parser_parseTokens(ParserHandle_t parser, MainFrameHandle_t root, const VectorHandler_t tokenVector);
bool Parser_destroy(ParserHandle_t parser);

#endif // UTILITY_PARSER_PARSER_H_