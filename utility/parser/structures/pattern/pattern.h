/**
 * @file pattern.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-01
 */
#ifndef UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_
#define UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_
#include "../../../tokenizer/token/token_database/token_types.h"

static const TokenType PATTERN_LIBRARY_IMPORT[] = {MODULE_IMPORT, LITTERAL, NAMING, LITTERAL, SEMICOLON};   // example: import "MainFrame";



#endif // UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_