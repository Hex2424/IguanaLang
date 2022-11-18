/**
 * @file single_operator.h
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

#ifndef UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_SINGLE_OPERATOR_H_
#define UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_SINGLE_OPERATOR_H_
#include "../../../../tokenizer/token/token_database/token_types.h"

typedef struct
{
    TokenType_t operatorTokenType;
}Operator_t;

typedef Operator_t* OperatorHandle_t;


#endif // UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_SINGLE_OPERATOR_H_