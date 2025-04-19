/**
 * @file method.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-07
 */

#ifndef UTILITY_PARSER_STRUCTURES_METHOD_METHOD_H_
#define UTILITY_PARSER_STRUCTURES_METHOD_METHOD_H_

#include "../variable/variable.h"
#include "../object_type/object_type.h"
#include <vector.h>
#include "method_scope/local_scope.h"

typedef struct
{
    Accessibility_t accessType : 4;
    char* methodName;
    VectorHandler_t parameters;
    VariableObjectHandle_t returnVariable;
    LocalScopeObject_t body;
    bool containsBody : 1;
    bool hasInfinityParams : 1;
}MethodObject_t;

typedef MethodObject_t* MethodObjectHandle_t;


#endif // UTILITY_PARSER_STRUCTURES_METHOD_METHOD_H_