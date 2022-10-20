/**
 * @file import_object.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-20
 */

#ifndef UTILITY_PARSER_STRUCTURES_IMPORT_OBJECT_IMPORT_OBJECT_H_
#define UTILITY_PARSER_STRUCTURES_IMPORT_OBJECT_IMPORT_OBJECT_H_
#include "../../../global_config/global_config.h"
#include <stdint.h>
#include "stdbool.h"

typedef struct
{
    char id[OBJECT_ID_LENGTH + 1]; // +1 for NULL terminator
}SpecialObjectID_t;

typedef struct
{
    char* name;
    SpecialObjectID_t objectId;
    char* realPath;
}ImportObject_t;


typedef ImportObject_t* ImportObjectHandle_t;

bool ImportObject_generateRandomIDForObject(ImportObjectHandle_t importObject);

#endif // UTILITY_PARSER_STRUCTURES_IMPORT_OBJECT_IMPORT_OBJECT_H_
