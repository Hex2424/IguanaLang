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

typedef struct 
{
    const char* expression;
    const TokenType_t type;
}BindingType_t;

// NOTE: Tokens in bindings table must have putted sequential way by token type, for valid compiler messages

static const BindingType_t bindingsTable_[] =
    {
        {"int", INTEGER_TYPE},                              
        {";", SEMICOLON},
        {"", NUMBER_VALUE},
        {"", NAMING},
        {"{", BRACKET_START},
        {"}", BRACKET_END},
        {":", COLON},
        {">", ARROW_RIGHT},
        {"(", BRACKET_ROUND_START},
        {")", BRACKET_ROUND_END},
        {"import", MODULE_IMPORT},
        {"!", NOT},
        {"=", EQUAL},
        {"+", OPERATOR_PLUS},
        {"-", OPERATOR_MINUS},
        {"/", OPERATOR_DIVIDE},
        {"*", OPERATOR_MULTIPLY},
        {"%", OPERATOR_MODULUS},
        {"when", LOOP_WHEN},
        {",", COMMA},
        {"<", ARROW_LEFT},
        {"@", NOTATION},
        {".", DOT_SYMBOL},
        {"\"", LITTERAL}
    };

#endif // UTILITY_TOKENIZER_TOKEN_TOKEN_DATABASE_TOKEN_BINDINGS_H_