/**
 * @file expressions.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-11-12
 */
#ifndef UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSIONS_H_
#define UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSIONS_H_
#include "../expression/expression_list/method_call.h"

typedef enum
{
    METHOD_CALL,
    WHEN_LOOP,
    WHILE_LOOP,
    REPEAT_LOOP,
    IF_STATEMENT,
    GUARD_STATEMENT,
}ExpressionType_t;

typedef struct
{
    ExpressionType_t type;
    void* expressionObject;
}Expression_t;

typedef Expression_t* ExpressionHandle_t;


#endif // UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSIONS_H_