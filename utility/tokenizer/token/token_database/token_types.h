/**
 * @file token_types.h
 * 
 * MORE INFO ABOUT THE FILE'S CONTENTS
 * 
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
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
    BIT_TYPE,            // bit
    SEMICOLON,           // ;
    NUMBER_VALUE,        // 1
    NAMING,              // varName
    BRACKET_START,       // {
    BRACKET_END,         // }
    COLON,               // :
    ARROW_RIGHT,         // >
    BRACKET_ROUND_START, // (
    BRACKET_ROUND_END,   // )
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
    DOT_SYMBOL,          // .
    LITTERAL,            // "
    THIS,                // this
    OPERATOR_XOR,        // ^
    OPERATOR_AND,        // &
    OPERATOR_OR,         // |
    OPERATOR_NOT,        // ~
    ALLOC_STATIC_STACK,  // ss
    ALLOC_DYNAMIC_STACK, // ds
    ALLOC_DYNAMIC_HEAP,  // dh
    NONE                 // none
} TokenType_t;


typedef TokenType_t* TokenTypeHandler_t;

#endif

