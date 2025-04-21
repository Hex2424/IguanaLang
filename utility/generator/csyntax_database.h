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

#define SIZEOF_NOTERM(string) (sizeof(string) - 1)


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


#define TYPE_BIT64_DEF           "uint64_t"
#define TYPE_BIT32_DEF           "uint32_t"
#define TYPE_BIT16_DEF           "uint16_t"
#define TYPE_BIT8_DEF            "uint8_t"
#define TYPE_BIT0_DEF            "void"

#endif // UTILITY_GENERATOR_CSYNTAX_DATABASE_H_