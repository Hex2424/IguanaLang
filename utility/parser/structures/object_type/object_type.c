/**
 * @file object_type.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-12-06
 */

#include "object_type.h"

////////////////////////////////
// DEFINES
// static const char* const TAG = "OBJECT_TYPE";

////////////////////////////////
// PRIVATE CONSTANTS


const BindingNotationType_t notationBindingTable[] = 
{
    {"CField", CFIELD},
    {"Private", PRIVATE},
    {"Public", PUBLIC},
    {"Ignored", IGNORED}
};

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION

BindingNotationTypeHandle_t ObjectTypes_getNotationBindingById(const uint8_t pos)
{
    return (BindingNotationTypeHandle_t) (notationBindingTable + pos);
}

uint8_t ObjectTypes_getNotationTableSize()
{
    return sizeof(notationBindingTable) / sizeof(BindingNotationType_t);
}