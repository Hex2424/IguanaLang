/**
 * @file pattern.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana+ and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-01
 */
#ifndef UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_
#define UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_
#include "../../../tokenizer/token/token_database/token_types.h"

#define TYPE_SIZE_MACRO(PATTERN) \
sizeof(PATTERN) / sizeof(TokenType_t)




static const TokenType_t PATTERN_LIBRARY_IMPORT[] = {MODULE_IMPORT, LITTERAL, NAMING, LITTERAL, SEMICOLON};   // example: import "MainFrame";
static const TokenType_t PATTERN_VARIABLE_DECLARE[] = {INTEGER_TYPE, COLON, NUMBER_VALUE, NAMING, SEMICOLON}; // example: int:3 number;


#define PATTERN_LIBRARY_IMPORT_SIZE     TYPE_SIZE_MACRO(PATTERN_LIBRARY_IMPORT)
#define PATTERN_VARIABLE_DECLARE_SIZE   TYPE_SIZE_MACRO(PATTERN_VARIABLE_DECLARE)

#endif // UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_