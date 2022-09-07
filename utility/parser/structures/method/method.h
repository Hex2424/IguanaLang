/**
 * @file method.h
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

#ifndef UTILITY_PARSER_STRUCTURES_METHOD_METHOD_H_
#define UTILITY_PARSER_STRUCTURES_METHOD_METHOD_H_

#include "method_scope/local_scope.h"
#include "../variable/variable.h"
#include "../object_type/object_type.h"

typedef struct
{
    Accessibility_t accessType;
    VariableObjectHandle_t* parameters;
    LocalScopeObjectHandle_t body;

}MethodObject_t;

typedef MethodObject_t* MethodObjectHandle_t;


#endif // UTILITY_PARSER_STRUCTURES_METHOD_METHOD_H_