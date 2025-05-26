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

    EXP_TMP_VAR,
    EXP_ELEMENT_UNKNOWN_TYPE
}ExpElementType_t;


typedef enum
{
    SIMPLE_LINE,
    REPEAT_LOOP,
    WHEN_LOOP,
    GUARD_STATEMENT,
    RETURN_STATEMENT,
    
    EXP_UNKNOWN_TYPE
}ExpType_t;

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

    OP_SET,
    OP_CAST
}OperatorType_t;

typedef struct
{
    ExpElementType_t type;
    void* expressionElement;
}ExpElement_t;

typedef ExpElement_t* ExpElementHandle_t;

typedef struct
{
    ExpType_t expType;
    Vector_t expressionElementVector;
}Exp_t;

typedef Exp_t* ExpHandle_t;

typedef ExpElementHandle_t* ExpIterator_t;

bool ExpElement_isSymbolOperand(const ExpElementHandle_t symbol);
bool ExpElement_isSymbolOperator(const ExpElementHandle_t symbol);
ExpElementHandle_t ExpElement_createDynamic(void);
bool ExpElement_setType(ExpElementHandle_t expressionElement, const ExpElementType_t type);
bool ExpElement_setObject(ExpElementHandle_t expressionElement, const void* objectRef);
bool ExpElement_set(ExpElementHandle_t expressionElement, const ExpElementType_t type, const void* objectRef);

ExpElementType_t ExpElement_getType(ExpElementHandle_t expressionElement);
void* ExpElement_getObject(const ExpElementHandle_t expressionElement);

ExpHandle_t Expression_createDynamic(const ExpType_t type);
bool Expression_setType(ExpHandle_t expression, const ExpType_t type);
ExpType_t Expression_getType(const ExpHandle_t expression);
bool Expression_create(ExpHandle_t expression, const ExpType_t expressionType);
bool Expression_addElement(ExpHandle_t expression, const ExpElementHandle_t element);

ExpIterator_t Expression_iteratorFirst(const ExpHandle_t expression);
ExpIterator_t Expression_iteratorLast(const ExpHandle_t expression);

size_t Expression_size(const ExpHandle_t expression);


#endif // UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSIONS_H_