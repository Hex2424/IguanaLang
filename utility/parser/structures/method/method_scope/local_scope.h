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
typedef struct
{
    Hashmap_t localVariables;
    Hashmap_t objectAllocations;

    Vector_t expressions;
}LocalScopeObject_t;

typedef LocalScopeObject_t* LocalScopeObjectHandle_t;

#endif // UTILITY_PARSER_STRUCTURES_METHOD_METHOD_SCOPE_LOCAL_SCOPE_H_
