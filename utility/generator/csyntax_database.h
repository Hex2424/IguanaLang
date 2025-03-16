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

#define CONST_STRING                static const char* const
#define CONST_SYMBOL                static const char

// C KEYWORDS
CONST_STRING NDEF_KEYWORD =      "#ifndef";
CONST_STRING DEF_KEYWORD =       "#define";
CONST_STRING ENDIF_KEYWORD =     "#endif";
CONST_STRING INCLUDE_KEYWORD =   "#include";
CONST_STRING TYPEDEF_KEYWORD =   "typedef";
CONST_STRING STRUCT_KEYWORD =    "struct";
CONST_STRING UNSIGNED_KEYWORD =  "unsigned";

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

CONST_SYMBOL BRACKET_START_CHAR =        '{';
CONST_SYMBOL BRACKET_END_CHAR =          '}';
CONST_SYMBOL BRACKET_ROUND_START_CHAR =  '(';
CONST_SYMBOL BRACKET_ROUND_END_CHAR =    ')';
CONST_SYMBOL SEMICOLON_CHAR =            ';';
CONST_SYMBOL COMMA_CHAR =                ',';

CONST_SYMBOL END_LINE_CHAR =             '\n';
CONST_SYMBOL NULL_TERMINATOR_CHAR =      '\0';

// These to do operation in preprocessor

#define BRACKET_START_DEF         "{"
#define BRACKET_END_DEF           "}"
#define BRACKET_ROUND_START_DEF   "("
#define BRACKET_ROUND_END_DEF     ")"
#define SEMICOLON_DEF             ";"
#define COMMA_DEF                 ","

#define END_LINE_DEF              "\n"
#define NULL_TERMINATOR_DEF       "\0"
#define DOUBLE_QUOTE_DEF          "\""

// Mangling definitions
// TODO Make different for each compiler

#define MANGLE_ASM_WRAP_LINE(mangle_format) \
    "asm(\"" mangle_format "\");"

#define MANGLE_MAGIC_BYTE_DEF    "_Z"
#define MANGLE_NEST_ID_DEF       "N"
#define MANGLE_END_DEF           "E"

#endif // UTILITY_GENERATOR_CSYNTAX_DATABASE_H_