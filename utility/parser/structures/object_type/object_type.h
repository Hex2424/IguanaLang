/**
 * @file object_type.h
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


#ifndef UTILITY_PARSER_STRUCTURES_OBJECT_TYPE_OBJECT_TYPE_H_
#define UTILITY_PARSER_STRUCTURES_OBJECT_TYPE_OBJECT_TYPE_H_
#include <stdint.h>

typedef enum
{
    PRIVATE,
    PUBLIC,
    CFIELD,
    IGNORED,
    NO_NOTATION
}Accessibility_t;


typedef struct 
{
    const char* naming;
    Accessibility_t type;
}BindingNotationType_t;

typedef BindingNotationType_t* BindingNotationTypeHandle_t;

BindingNotationTypeHandle_t ObjectTypes_getNotationBindingById(const uint8_t pos);
uint8_t ObjectTypes_getNotationTableSize();

#endif // UTILITY_PARSER_STRUCTURES_OBJECT_TYPE_OBJECT_TYPE_H_