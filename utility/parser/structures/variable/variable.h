/**
 * @file variable.h
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

#ifndef UTILITY_PARSER_STRUCTURES_VARIABLE_VARIABLE_H_
#define UTILITY_PARSER_STRUCTURES_VARIABLE_VARIABLE_H_

#include "stdint.h"


typedef struct
{
    char* variableName;
    uint8_t bitpack;

}VariableObject_t;

typedef VariableObject_t* VariableObjectHandle_t;


#endif // UTILITY_PARSER_STRUCTURES_VARIABLE_VARIABLE_H_