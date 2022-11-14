/**
 * @file method_call.h
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
#ifndef UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_METHOD_CALL_H_
#define UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_METHOD_CALL_H_
#include "../../method/method.h"


typedef struct
{
    bool isMethodSelf;
    MethodObject_t method;
}ExMethodCall_t;

typedef ExMethodCall_t* ExMethodCallHandle_t;

#endif // UTILITY_PARSER_STRUCTURES_EXPRESSION_EXPRESSION_LIST_METHOD_CALL_H_