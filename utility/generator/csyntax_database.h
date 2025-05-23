/**
 * @file csyntax_database.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-15
 */

#ifndef UTILITY_GENERATOR_CSYNTAX_DATABASE_H_
#define UTILITY_GENERATOR_CSYNTAX_DATABASE_H_

#include <helper_macros.h>

#define CONST_STRING                static const char* const
#define CONST_SYMBOL                static const char

// C KEYWORDS

#define NDEF_KEYWORD_DEF            "#ifndef"
#define DEF_KEYWORD_DEF             "#define"
#define ENDIF_KEYWORD_DEF           "#endif"
#define INCLUDE_KEYWORD_DEF         "#include"
#define TYPEDEF_KEYWORD_DEF         "typedef"
#define STRUCT_KEYWORD_DEF          "struct"
#define UNSIGNED_KEYWORD_DEF        "unsigned"
#define EXTERN_KEYWORD_DEF          "extern"

#define INCLUDE_WRAP(lib) INCLUDE_KEYWORD_DEF "<" lib ".h>" END_LINE_DEF
#define TYPEDEF_WRAP(from, to) TYPEDEF_KEYWORD_DEF " " from " " to SEMICOLON_DEF

// IMPORTANT! database order should be from smallest to highest
CONST_STRING TYPE_BINDS[] = 
{
    "char",         // 0 - 8
    "short",        // 8 - 16
    "int",          // 16 - 24
    "int",          // 24 - 32
    "long long",    // 32 - 40
    "long long",    // 40 - 48
    "long long",    // 48 - 56
    "long long",    // 56 - 64
    "void"
};

// These to do operation runtime

// CONST_SYMBOL BRACKET_START_CHAR =        '{';
// CONST_SYMBOL BRACKET_END_CHAR =          '}';
// CONST_SYMBOL BRACKET_ROUND_START_CHAR =  '(';
// CONST_SYMBOL BRACKET_ROUND_END_CHAR =    ')';
// CONST_SYMBOL SEMICOLON_CHAR =            ';';
// CONST_SYMBOL COMMA_CHAR =                ',';

// CONST_SYMBOL END_LINE_CHAR =             '\n';
// CONST_SYMBOL NULL_TERMINATOR_CHAR =      '\0';

// These to do operation in preprocessor

#define BRACKET_START_DEF         "{"
#define BRACKET_END_DEF           "}"
#define BRACKET_ROUND_START_DEF   "("
#define BRACKET_ROUND_END_DEF     ")"
#define SEMICOLON_DEF             ";"
#define COMMA_DEF                 ","
#define BIT_DEF                   "bit"

#define END_LINE_DEF              "\n"
#define NULL_TERMINATOR_DEF       "\0"
#define DOUBLE_QUOTE_DEF          "\""

#define POINTER_SIGN_DEF          "*"            

// Mangling definitions
// TODO Make different for each compiler
#define ASM_HEADER_MANGLE         "asm(\""
#define ASM_FOOTER_MANGLE         "\")"


#define MANGLE_MAGIC_BYTE_DEF    "_Z"
#define MANGLE_NEST_ID_DEF       "N"
#define MANGLE_END_DEF           "E"

#define MANGLE_TYPE_VOID_DEF     "v"
#define MANGLE_TYPE_INT_DEF      "i"
#define MANGLE_TYPE_SHORT_DEF    "s"
#define MANGLE_TYPE_CHAR_DEF     "c"


// Operators supported by C language

#define C_OPERATOR_PLUS_DEF      "+"
#define C_OPERATOR_MINUS_DEF     "-"
#define C_OPERATOR_DIVIDE_DEF    "/"
#define C_OPERATOR_MULTIPLY_DEF  "*"
#define C_OPERATOR_MODULUS_DEF   "%"
#define C_OPERATOR_EQUAL_DEF     "="

// Binary Operators supported by C language

#define C_OPERATOR_BIN_XOR_DEF          "^"
#define C_OPERATOR_BIN_AND_DEF          "&"
#define C_OPERATOR_BIN_OR_DEF           "|"
#define C_OPERATOR_BIN_NOT_DEF          "~"

#define C_OPERATOR_BIN_OR_EQUAL_DEF     "|="

#if ARCHITECTURE_DEFAULT_BITS == 64
    #define BIT_TYPE_EXP              TYPE_BIT64_EXP
    #define BIT_SIZE_BITPACK          64
#elif ARCHITECTURE_DEFAULT_BITS == 32
    #define BIT_TYPE_EXP              TYPE_BIT32_EXP
    #define BIT_SIZE_BITPACK          32
#elif ARCHITECTURE_DEFAULT_BITS == 16
    #define BIT_TYPE_EXP              TYPE_BIT16_EXP
    #define BIT_SIZE_BITPACK          16
#elif ARCHITECTURE_DEFAULT_BITS == 8
    #define BIT_TYPE_EXP              TYPE_BIT8_EXP
    #define BIT_SIZE_BITPACK          8
#else
    #define BIT_TYPE_EXP              TYPE_BIT32_EXP
    #define BIT_SIZE_BITPACK          32
#endif

#define BIT_TYPE_DEF              STRINGIFY(BIT_TYPE_EXP)


#define PARAM_TYPE_DEF            BITPACK_TYPE_NAME POINTER_SIGN_DEF
#define BITPACK_TYPE_TYPEDEF_DEF  TYPEDEF_WRAP(BIT_TYPE_DEF, BITPACK_TYPE_NAME)


#define TYPE_BIT64_EXP            uint64_t
#define TYPE_BIT32__EXP           uint32_t
#define TYPE_BIT16__EXP           uint16_t
#define TYPE_BIT8_EXP             uint8_t
#define TYPE_BIT0_EXP             void

#define TYPE_BIT64_DEF           STRINGIFY(TYPE_BIT64_EXP)
#define TYPE_BIT32_DEF           STRINGIFY(TYPE_BIT32_EXP)
#define TYPE_BIT16_DEF           STRINGIFY(TYPE_BIT16_EXP)
#define TYPE_BIT8_DEF            STRINGIFY(TYPE_BIT8_EXP)
#define TYPE_BIT0_DEF            STRINGIFY(TYPE_BIT0_EXP)


#endif // UTILITY_GENERATOR_CSYNTAX_DATABASE_H_