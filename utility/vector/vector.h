/**
 * @file vector.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-06
 */

#ifndef UTILITY_VECTOR_VECTOR_H_
#define UTILITY_VECTOR_VECTOR_H_

#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "../misc/safety_macros.h"


#define EXPANDABLE_CONSTANT_DEFAULT     (1.0f / 3.0f)

typedef struct
{
    bool containsVectors;
    void** expandable;
    size_t currentSize;
    size_t availableSize;
    float expandableConstant;

}Vector_t;

typedef struct
{
    size_t initialSize;
    float expandableConstant;
    bool containsVectors;
}InitialSettings_t;

typedef Vector_t* VectorHandler_t;
typedef InitialSettings_t* InitialSettingsHandler_t;


VectorHandler_t Vector_createDynamic(const InitialSettingsHandler_t initialSettings);

bool Vector_create(VectorHandler_t object, const InitialSettingsHandler_t initialSettings);
bool Vector_append(VectorHandler_t object, const void* dataObject);
bool Vector_fit(VectorHandler_t object);
bool Vector_destroy(VectorHandler_t object);
void Vector_print(VectorHandler_t object);
#endif // UTILITY_VECTOR_VECTOR_H_