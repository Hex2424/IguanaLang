/**
 * @file pattern.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-01
 */
#ifndef UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_
#define UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_

#include "../../../tokenizer/token/token_database/token_types.h"

#define TYPE_SIZE_MACRO(PATTERN) \
sizeof(PATTERN) / sizeof(TokenType_t)

#define Pattern_t static const TokenType_t


Pattern_t PATTERN_LIBRARY_IMPORT[] =        {MODULE_IMPORT, LITTERAL, NAMING, LITTERAL, SEMICOLON};   // example: import "MainFrame";
Pattern_t PATTERN_DECLARE[] =               {BIT_TYPE, COLON, NUMBER_VALUE, NAMING};              // example: int:3 object
Pattern_t PATTERN_OBJECT_DECLARATION[] =    {NAMING, NAMING};                                         // example: Object object
Pattern_t PATTERN_SEMICOLON[] =             {SEMICOLON};                                              // example: ;
Pattern_t PATTERN_BRACKET_ROUND_START[] =   {BRACKET_ROUND_START};                                    // example: (
Pattern_t PATTERN_COMMA[] =                 {COMMA};                                                  // example: ,
Pattern_t PATTERN_BRACKET_ROUND_END[] =     {BRACKET_ROUND_END};                                      // example: )

#define PATTERN_LIBRARY_IMPORT_SIZE         TYPE_SIZE_MACRO(PATTERN_LIBRARY_IMPORT)
#define PATTERN_DECLARE_SIZE                TYPE_SIZE_MACRO(PATTERN_DECLARE)
#define PATTERN_OBJECT_DECLARATION_SIZE     TYPE_SIZE_MACRO(PATTERN_OBJECT_DECLARATION)
#define PATTERN_SEMICOLON_SIZE              TYPE_SIZE_MACRO(PATTERN_SEMICOLON)
#define PATTERN_BRACKET_ROUND_START_SIZE    TYPE_SIZE_MACRO(PATTERN_BRACKET_ROUND_START)
#define PATTERN_COMMA_SIZE                  TYPE_SIZE_MACRO(PATTERN_COMMA)
#define PATTERN_BRACKET_ROUND_END_SIZE      TYPE_SIZE_MACRO(PATTERN_BRACKET_ROUND_END)


#endif // UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_