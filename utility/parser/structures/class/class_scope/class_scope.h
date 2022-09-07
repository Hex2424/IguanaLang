/**
 * @file class_scope.h
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

#ifndef UTILITY_PARSER_STRUCTURES_CLASS_CLASS_SCOPE_CLASS_SCOPE_H_
#define UTILITY_PARSER_STRUCTURES_CLASS_CLASS_SCOPE_CLASS_SCOPE_H_

#include "../../variable/variable.h"
#include "../../method/method.h"

typedef struct
{
    VariableObjectHandle_t* variables;
    MethodObjectHandle_t* methods;

}ClassScopeObject_t;

typedef ClassScopeObject_t* ClassScopeObjectHandle_t;

#endif // UTILITY_PARSER_STRUCTURES_CLASS_CLASS_SCOPE_CLASS_SCOPE_H_
