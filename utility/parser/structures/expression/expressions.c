/**
 * @file expressions.c
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

#include "expressions.h"

////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS


////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool Expression_isSymbolOperand(const ExpressionHandle_t symbol)
{
    return (symbol->type == EXP_METHOD_CALL) ||
        (symbol->type == EXP_CONST_NUMBER)   ||
        (symbol->type == EXP_VARIABLE);

    // TODO: Add binary operators
}

bool Expression_isSymbolOperator(const ExpressionHandle_t symbol)
{
    return (symbol->type == EXP_OPERATOR);

    // TODO: Add binary operators
}