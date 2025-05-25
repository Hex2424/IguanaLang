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
static const char* const TAG = "EXPRESSIONS";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

bool ExpElement_isSymbolOperand(const ExpElementHandle_t symbol)
{
    return (symbol->type == EXP_METHOD_CALL) ||
        (symbol->type == EXP_CONST_NUMBER)   ||
        (symbol->type == EXP_VARIABLE);

    // TODO: Add binary operators
}

bool ExpElement_isSymbolOperator(const ExpElementHandle_t symbol)
{
    return (symbol->type == EXP_OPERATOR);

    // TODO: Add binary operators
}

ExpElementHandle_t ExpElement_createDynamic(void)
{
    ExpElementHandle_t expressionSymbol;

    ALLOC_CHECK(expressionSymbol, sizeof(ExpElement_t), NULL);

    return expressionSymbol;
}

bool ExpElement_setType(ExpElementHandle_t expressionElement, const ExpElementType_t type)
{
    NULL_GUARD(expressionElement, ERROR, Log_e(TAG, "ExpElement_setType Expression element passed as NULL"));

    expressionElement->type = type;
    
    return SUCCESS;
}

ExpElementType_t ExpElement_getType(ExpElementHandle_t expressionElement)
{
    NULL_GUARD(expressionElement, EXP_ELEMENT_UNKNOWN_TYPE, Log_e(TAG, "ExpElement_getType Expression element passed as NULL"));

    return expressionElement->type;
}

bool ExpElement_setObject(ExpElementHandle_t expressionElement, const void* objectRef)
{
    NULL_GUARD(expressionElement, ERROR, Log_e(TAG, "Expression element passed as NULL"));

    expressionElement->expressionElement = (void*) objectRef;
    
    return SUCCESS;
}

void* ExpElement_getObject(const ExpElementHandle_t expressionElement)
{
    NULL_GUARD(expressionElement, NULL, Log_e(TAG, "ExpElement_getObject Expression element passed as NULL"));

    return expressionElement->expressionElement;
}


bool ExpElement_set(ExpElementHandle_t expressionElement, const ExpElementType_t type, const void* objectRef)
{
    NULL_GUARD(expressionElement, ERROR, Log_e(TAG, "ExpElement_set Expression element passed as NULL"));
    expressionElement->type = type;
    expressionElement->expressionElement = (void*) objectRef;
    return SUCCESS;
}

ExpHandle_t Expression_createDynamic(const ExpType_t type)
{
    ExpHandle_t expression;

    ALLOC_CHECK(expression, sizeof(Exp_t), NULL);

    if(!Expression_create(expression, type))
    {
        Log_e(TAG, "Failed to create dynamic expression");
        return NULL;
    }

    return expression;
}


bool Expression_setType(ExpHandle_t expression, const ExpType_t type)
{
    NULL_GUARD(expression, ERROR, Log_e(TAG, "Expression_setType Failed to set expression type its NULL"));
    expression->expType = type;

    return SUCCESS;
}


bool Expression_getType(const ExpHandle_t expression)
{
    NULL_GUARD(expression, EXP_UNKNOWN_TYPE, Log_e(TAG, "Expression_getType Failed to get type since expression is NULL for some reason"));

    return expression->expType;
}


bool Expression_create(ExpHandle_t expression, const ExpType_t expressionType)
{
    NULL_GUARD(expression, ERROR, Log_e(TAG, "Expression_create Expression passed as NULL"));

    expression->expType = expressionType;

    InitialSettings_t settingsVector;

    settingsVector.containsVectors = false;
    settingsVector.expandableConstant = EXPANDABLE_CONSTANT_DEFAULT;
    settingsVector.initialSize = 10;

    if(!Vector_create(&expression->expressionElementVector, &settingsVector))
    {
        Log_e(TAG, "Error creating expression vector");
        return ERROR;
    }

    return SUCCESS;
}

bool Expression_addElement(ExpHandle_t expression, const ExpElementHandle_t element)
{
    NULL_GUARD(expression, ERROR, Log_e(TAG, "Expression_addElement Expression passed as NULL"));

    if(!Vector_append(&expression->expressionElementVector, element))
    {
        Log_e(TAG, "Failed to append expression element to expression");
        return ERROR;
    }

    return SUCCESS;
}

ExpIterator_t Expression_iteratorFirst(const ExpHandle_t expression)
{
    NULL_GUARD(expression, NULL, Log_e(TAG, "Expression_iteratorFirst Expression passed as NULL"));

    return (ExpIterator_t) expression->expressionElementVector.expandable;
}


ExpIterator_t Expression_iteratorLast(const ExpHandle_t expression)
{
    NULL_GUARD(expression, NULL, Log_e(TAG, "Expression_iteratorLast Expression passed as NULL"));
    
    return (ExpIterator_t) expression->expressionElementVector.expandable + expression->expressionElementVector.currentSize;
}

size_t Expression_size(const ExpHandle_t expression)
{
    NULL_GUARD(expression, -1, Log_e(TAG, "Expression_size Expression passed as NULL"));

    return expression->expressionElementVector.currentSize;
}