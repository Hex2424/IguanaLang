/**
 * @file token_types.h
 * 
 * MORE INFO ABOUT THE FILE'S CONTENTS
 * 
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 * 
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 * 
 * @date 2022-08-05
 */


#ifndef UTILITY_TOKENIZER_TOKEN_TOKEN_DATABASE_TOKEN_TYPES_H
#define UTILITY_TOKENIZER_TOKEN_TOKEN_DATABASE_TOKEN_TYPES_H

// #include <stdbool.h>
// #include <stdint.h>
// #include <stdlib.h>

typedef enum TokenType
{
    INTEGER_TYPE,        // int
    SEMICOLON,           // ;
    NUMBER_VALUE,        // 1
    NAMING,              // varName
    BRACKET_START,       // {
    BRACKET_END,         // }
    COLON,               // :
    ARROW_RIGHT,         // >
    BRACKET_ROUND_START, // (
    BRACKET_ROUND_END,   // )
    MODULE_IMPORT,       // import
    MODULE_NAME,         // # sys
    NOT,                 // !
    EQUAL,               // =
    OPERATOR_PLUS,       // +
    OPERATOR_MINUS,      // -
    OPERATOR_DIVIDE,     // /
    OPERATOR_MULTIPLY,   // *
    OPERATOR_MODULUS,    // %
    LOOP_WHEN,           // when
    COMMA,               // ,
    ARROW_LEFT,          // <
    NOTATION,            // @
} TokenType_t;

#endif

