/**
 * @file local_scope.h
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
#ifndef UTILITY_PARSER_STRUCTURES_METHOD_METHOD_SCOPE_LOCAL_SCOPE_H_
#define UTILITY_PARSER_STRUCTURES_METHOD_METHOD_SCOPE_LOCAL_SCOPE_H_

#include <hashmap.h>
#include <vector.h>
#include <platform_specific.h>

typedef struct
{
    Hashmap_t localVariables;
    BitpackSize_t sizeBits; 
    Vector_t expressionList;

    // TODO: make this more flexible accross scopes in future and not fixed refs
    HashmapHandle_t objectVarsRef;
    VectorHandler_t paramsVarsRef;
}LocalScopeObject_t;

typedef LocalScopeObject_t* LocalScopeObjectHandle_t;

#endif // UTILITY_PARSER_STRUCTURES_METHOD_METHOD_SCOPE_LOCAL_SCOPE_H_
