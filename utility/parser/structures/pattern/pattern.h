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


Pattern_t PATTERN_DECLARE[] =               {BIT_TYPE, COLON, NUMBER_VALUE, NAMING};                  // example: bit:3 object
Pattern_t PATTERN_SEMICOLON[] =             {SEMICOLON};                                              // example: ;
Pattern_t PATTERN_BRACKET_ROUND_START[] =   {BRACKET_ROUND_START};                                    // example: (
Pattern_t PATTERN_COMMA[] =                 {COMMA};                                                  // example: ,
Pattern_t PATTERN_BRACKET_ROUND_END[] =     {BRACKET_ROUND_END};                                      // example: )
Pattern_t PATTERN_CAST_TYPE_CONST[] =       {BIT_TYPE, COLON, NUMBER_VALUE, BRACKET_ROUND_START, NUMBER_VALUE, BRACKET_ROUND_END};   // example: bit:1(6)
Pattern_t PATTERN_CAST_TYPE_VAR[] =         {BIT_TYPE, COLON, NUMBER_VALUE, BRACKET_ROUND_START, NAMING, BRACKET_ROUND_END};         // example: bit:32(variable)
Pattern_t PATTERN_VAR_TYPE[] =              {BIT_TYPE, COLON, NUMBER_VALUE};                          // example: bit:32

Pattern_t PATTERN_STATIC_STACK_OBJECT[] =   {ALLOC_STATIC_STACK, NAMING, NAMING};                     // example: ss SOBJECT obj
Pattern_t PATTERN_DYNAMIC_STACK_OBJECT[] =  {ALLOC_DYNAMIC_STACK, NAMING, NAMING};                    // example: ds DOBJECT obj
Pattern_t PATTERN_DYNAMIC_HEAP_OBJECT[] =   {ALLOC_DYNAMIC_HEAP, NAMING, NAMING};                     // example: dh DOBJECT obj

Pattern_t PATTERN_FILE_OBJECT_DEFINE[] =    {FILE_OBJECT, NAMING, BRACKET_ROUND_START};               // example file filename(


#define PATTERN_LIBRARY_IMPORT_SIZE         TYPE_SIZE_MACRO(PATTERN_LIBRARY_IMPORT)
#define PATTERN_DECLARE_SIZE                TYPE_SIZE_MACRO(PATTERN_DECLARE)
#define PATTERN_SEMICOLON_SIZE              TYPE_SIZE_MACRO(PATTERN_SEMICOLON)
#define PATTERN_BRACKET_ROUND_START_SIZE    TYPE_SIZE_MACRO(PATTERN_BRACKET_ROUND_START)
#define PATTERN_COMMA_SIZE                  TYPE_SIZE_MACRO(PATTERN_COMMA)
#define PATTERN_BRACKET_ROUND_END_SIZE      TYPE_SIZE_MACRO(PATTERN_BRACKET_ROUND_END)
#define PATTERN_CAST_TYPE_CONST_SIZE        TYPE_SIZE_MACRO(PATTERN_CAST_TYPE_CONST)
#define PATTERN_CAST_TYPE_VAR_SIZE          TYPE_SIZE_MACRO(PATTERN_CAST_TYPE_VAR)

#define PATTERN_STATIC_STACK_OBJECT_SIZE    TYPE_SIZE_MACRO(PATTERN_STATIC_STACK_OBJECT)
#define PATTERN_DYNAMIC_STACK_OBJECT_SIZE   TYPE_SIZE_MACRO(PATTERN_DYNAMIC_STACK_OBJECT)
#define PATTERN_DYNAMIC_HEAP_OBJECT_SIZE    TYPE_SIZE_MACRO(PATTERN_DYNAMIC_HEAP_OBJECT)
#define PATTERN_FILE_OBJECT_DEFINE_SIZE     TYPE_SIZE_MACRO(PATTERN_FILE_OBJECT_DEFINE)
#define PATTERN_VAR_TYPE_SIZE               TYPE_SIZE_MACRO(PATTERN_VAR_TYPE)


#endif // UTILITY_PARSER_STRUCTURES_PATTERN_PATTERN_H_