/**
 * @file token_bindings.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
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


static const BindingType_t bindingsTable_[] =
    {
        {";", SEMICOLON},
        {"import", MODULE_IMPORT},
        {"int", INTEGER_TYPE},
        {":", COLON},
        {"{", BRACKET_START},
        {"}", BRACKET_END},
        {"}", BRACKET_END},
        {"when", LOOP_WHEN},
        {",", COMMA},
        {"%", OPERATOR_MODULUS},
        {"+", OPERATOR_PLUS},
        {"-", OPERATOR_MINUS},
        {"/", OPERATOR_DIVIDE},
        {"*", OPERATOR_MULTIPLY},
        {"=", EQUAL},
        {"!", NOT},
        {"(", BRACKET_ROUND_START},
        {")", BRACKET_ROUND_END},
        {">", ARROW_RIGHT},
        {"@", NOTATION},
        {".", DOT_SYMBOL},
        {"\"", LITTERAL}
    };


#endif // UTILITY_TOKENIZER_TOKEN_TOKEN_DATABASE_TOKEN_BINDINGS_H_