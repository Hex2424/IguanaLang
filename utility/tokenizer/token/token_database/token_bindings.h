/**
 * @file token_bindings.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-05
 */

#ifndef UTILITY_TOKENIZER_TOKEN_TOKEN_DATABASE_TOKEN_BINDINGS_H_
#define UTILITY_TOKENIZER_TOKEN_TOKEN_DATABASE_TOKEN_BINDINGS_H_

#include "token_types.h"
#include <stdint.h>

#define DECLARE_TYPE(value, typeMacro) {value, typeMacro, sizeof(value) - 1}

typedef struct 
{
    const char* expression;
    const TokenType_t type;
    const uint8_t size;
}BindingType_t;

// NOTE: Tokens in bindings table must have putted sequential way by token type, for valid compiler messages

static const BindingType_t bindingsTable_[] =
    {
        [BIT_TYPE] = DECLARE_TYPE("bit", BIT_TYPE),
        [SEMICOLON] = DECLARE_TYPE(";", SEMICOLON),
        [NUMBER_VALUE] = DECLARE_TYPE("", NUMBER_VALUE),
        [NAMING] = DECLARE_TYPE("", NAMING),
        [BRACKET_START] = DECLARE_TYPE("{", BRACKET_START),
        [BRACKET_END] = DECLARE_TYPE("}", BRACKET_END),
        [COLON] = DECLARE_TYPE(":", COLON),
        [ARROW_RIGHT] = DECLARE_TYPE(">", ARROW_RIGHT),
        [BRACKET_ROUND_START] = DECLARE_TYPE("(", BRACKET_ROUND_START),
        [BRACKET_ROUND_END] = DECLARE_TYPE(")", BRACKET_ROUND_END),
        [NOT] = DECLARE_TYPE("!", NOT),
        [EQUAL] = DECLARE_TYPE("=", EQUAL),
        [OPERATOR_PLUS] = DECLARE_TYPE("+", OPERATOR_PLUS),
        [OPERATOR_MINUS] = DECLARE_TYPE("-", OPERATOR_MINUS),
        [OPERATOR_DIVIDE] = DECLARE_TYPE("/", OPERATOR_DIVIDE),
        [OPERATOR_MULTIPLY] = DECLARE_TYPE("*", OPERATOR_MULTIPLY),
        [OPERATOR_MODULUS] = DECLARE_TYPE("%", OPERATOR_MODULUS),
        [OPERATOR_XOR] = DECLARE_TYPE("^", OPERATOR_XOR),
        [OPERATOR_AND] = DECLARE_TYPE("&", OPERATOR_AND),
        [OPERATOR_OR] = DECLARE_TYPE("|", OPERATOR_OR),
        [OPERATOR_NOT] = DECLARE_TYPE("~", OPERATOR_NOT),
        [LOOP_WHEN] = DECLARE_TYPE("when", LOOP_WHEN),
        [COMMA] = DECLARE_TYPE(",", COMMA),
        [ARROW_LEFT] = DECLARE_TYPE("<", ARROW_LEFT),
        [NOTATION] = DECLARE_TYPE("@", NOTATION),
        [DOT_SYMBOL] = DECLARE_TYPE(".", DOT_SYMBOL),
        [LITTERAL] = DECLARE_TYPE("\"", LITTERAL),
        [THIS] = DECLARE_TYPE("this", THIS),
        [ALLOC_STATIC_STACK] = DECLARE_TYPE("ss", ALLOC_STATIC_STACK),
        [ALLOC_DYNAMIC_STACK] = DECLARE_TYPE("ds", ALLOC_DYNAMIC_STACK),
        [ALLOC_DYNAMIC_HEAP] = DECLARE_TYPE("dh", ALLOC_DYNAMIC_HEAP),
        [NONE] = DECLARE_TYPE("none", NONE)
    };

#endif // UTILITY_TOKENIZER_TOKEN_TOKEN_DATABASE_TOKEN_BINDINGS_H_