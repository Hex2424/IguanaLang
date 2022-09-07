/**
 * @file class.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-07
 */

#ifndef UTILITY_PARSER_STRUCTURES_CLASS_CLASS_H_
#define UTILITY_PARSER_STRUCTURES_CLASS_CLASS_H_

#include "class_scope/class_scope.h"
#include "../variable/variable.h"


typedef struct
{
    Accessibility_t accessType;
    VariableObjectHandle_t* parameters;
    ClassScopeObjectHandle_t body;

}ClassObject_t;

typedef ClassObject_t* ClassObjectHandle_t;

#endif // UTILITY_PARSER_STRUCTURES_CLASS_CLASS_H_