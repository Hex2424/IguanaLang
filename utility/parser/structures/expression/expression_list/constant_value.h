/**
 * @file constant_value.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-11-18
 */

#ifndef UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_CONSTANT_VALUE_H_
#define UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_CONSTANT_VALUE_H_

#include "../../../../tokenizer/token/token_database/token_types.h"

typedef struct
{
    char* valueAsString;
}ConstantNumber_t;

typedef ConstantNumber_t* ConstantNumberHandle_t;

#endif // UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_CONSTANT_VALUE_H_