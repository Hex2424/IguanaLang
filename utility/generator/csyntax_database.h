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

#define CONST_STRING                static const char*
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

CONST_SYMBOL BRACKET_START =        '{';
CONST_SYMBOL BRACKET_END =          '}';
CONST_SYMBOL BRACKET_ROUND_START =  '(';
CONST_SYMBOL BRACKET_ROUND_END =    ')';
CONST_SYMBOL SEMICOLON =            ';';
CONST_SYMBOL COMMA =                ',';

CONST_SYMBOL END_LINE =             '\n';
CONST_SYMBOL NULL_TERMINATOR =      '\0';

// #define CCHAR_SIZE               8
// #define CSHORT_SIZE              16
// #define CINT_SIZE                32
// #define CLONG_SIZE               64




#endif // UTILITY_GENERATOR_CSYNTAX_DATABASE_H_