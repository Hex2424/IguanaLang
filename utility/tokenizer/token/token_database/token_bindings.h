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
        DECLARE_TYPE("bit", BIT_TYPE),
        DECLARE_TYPE(";", SEMICOLON),
        DECLARE_TYPE("", NUMBER_VALUE),
        DECLARE_TYPE("", NAMING),
        DECLARE_TYPE("{", BRACKET_START),
        DECLARE_TYPE("}", BRACKET_END),
        DECLARE_TYPE(":", COLON),
        DECLARE_TYPE(">", ARROW_RIGHT),
        DECLARE_TYPE("(", BRACKET_ROUND_START),
        DECLARE_TYPE(")", BRACKET_ROUND_END),
        DECLARE_TYPE("import", MODULE_IMPORT),
        DECLARE_TYPE("!", NOT),
        DECLARE_TYPE("=", EQUAL),
        DECLARE_TYPE("+", OPERATOR_PLUS),
        DECLARE_TYPE("-", OPERATOR_MINUS),
        DECLARE_TYPE("/", OPERATOR_DIVIDE),
        DECLARE_TYPE("*", OPERATOR_MULTIPLY),
        DECLARE_TYPE("%", OPERATOR_MODULUS),
        DECLARE_TYPE("when", LOOP_WHEN),
        DECLARE_TYPE(",", COMMA),
        DECLARE_TYPE("<", ARROW_LEFT),
        DECLARE_TYPE("@", NOTATION),
        DECLARE_TYPE(".", DOT_SYMBOL),
        DECLARE_TYPE("\"", LITTERAL),
        DECLARE_TYPE("this", THIS)
    };

#endif // UTILITY_TOKENIZER_TOKEN_TOKEN_DATABASE_TOKEN_BINDINGS_H_