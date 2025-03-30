/**
 * @file fileobject.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2025-03-30
 */

#ifndef UTILITY_PARSER_STRUCTURES_FILEOBJECT_FILEOBJECT_H_
#define UTILITY_PARSER_STRUCTURES_FILEOBJECT_FILEOBJECT_H_

#include <stdint.h>
#include <stdbool.h>
#include <vector.h>

typedef struct
{
    char* objectActualName;
    Vector_t bitTypes;
}FileObject_t;

typedef FileObject_t* FileObjectHandle_t;


#endif // UTILITY_PARSER_STRUCTURES_FILEOBJECT_FILEOBJECT_H_