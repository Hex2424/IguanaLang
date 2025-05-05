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
    EXP_OPERATOR,
    EXP_METHOD_CALL,
    EXP_CONST_NUMBER,
    EXP_VARIABLE,
    EXP_PARENTHESES_LEFT,
    EXP_PARENTHESES_RIGHT,

    EXP_TMP_VAR
}ExpressionType_t;


typedef enum
{
    OP_PLUS,
    OP_MINUS,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULUS,
    OP_NOT,
    OP_AND,
    OP_OR,

    OP_BIN_XOR,
    OP_BIN_NOT,
    OP_BIN_AND,
    OP_BIN_OR,

    OP_SET
}OperatorType_t;

typedef struct
{
    ExpressionType_t type;
    void* expressionObject;
}Expression_t;

typedef Expression_t* ExpressionHandle_t;


bool Expression_isSymbolOperand(const ExpressionHandle_t symbol);
bool Expression_isSymbolOperator(const ExpressionHandle_t symbol);


#endif // UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSIONS_H_