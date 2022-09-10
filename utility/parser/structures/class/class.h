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

#include"../method/method.h"
#include "../object_type/object_type.h"


typedef struct Class ClassObject_t;
typedef struct ClassScope ClassScopeObject_t;

typedef ClassObject_t* ClassObjectHandle_t;
typedef ClassScopeObject_t* ClassScopeObjectHandle_t;

struct Class
{
    VariableObjectHandle_t* variables;
    MethodObjectHandle_t* methods;
    ClassObjectHandle_t* insiderClasses;
};


struct ClassScope
{
    Accessibility_t accessType;
    VariableObjectHandle_t* parameters;
    ClassScopeObjectHandle_t body;

};

#endif // UTILITY_PARSER_STRUCTURES_CLASS_CLASS_H_