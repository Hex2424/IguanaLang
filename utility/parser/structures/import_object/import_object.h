
#ifndef UTILITY_PARSER_STRUCTURES_IMPORT_OBJECT_IMPORT_OBJECT_H_
#define UTILITY_PARSER_STRUCTURES_IMPORT_OBJECT_IMPORT_OBJECT_H_
#include "../../../global_config/global_config.h"

typedef struct
{
    char id[OBJECT_ID_LENGTH + 2]; // +2 for extension putting optimzation
}SpecialObjectID_t;

typedef struct
{
    char* name;
    SpecialObjectID_t objectId;
}ImportObject_t;


typedef ImportObject_t* ImportObjectHandle_t;


#endif // UTILITY_PARSER_STRUCTURES_IMPORT_OBJECT_IMPORT_OBJECT_H_
