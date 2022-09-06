/**
 * @file vector.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-06
 */
#include "vector.h"
#include "../logger/logger.h"


////////////////////////////////
// DEFINES
#define INITIAL_SIZE_DEFAULT    5


#define VECTOR_NULL_PRINT \
    Log_e(TAG, "Vector object is null, returning with error...")
////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "VECTOR";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION


/**
 * @brief Public method for initializing Vector object
 * 
 * @param object[in / out] pointer to vector structure
 * @param initialSettings[in] initial advanced vector object settings for manual setup
 * @return Success state
 */

bool Vector_create(VectorHandler_t object, const InitialSettingsHandler_t initialSettings)
{
    NULL_GUARD(object, ERROR, VECTOR_NULL_PRINT);

    object->currentSize = 0;

    if(object->currentSize != 0)
    {
        Log_w(TAG, "Passed not new vector object, destroying old one...");

        if(!Vector_destroy(object))
        {
            Log_e(TAG, "Failed to destroy old vector object");
            return ERROR;
        }
    }

    if(initialSettings != NULL)
    {
        Log_i(TAG, "Found vector initial settings");

        object->expandable = malloc(initialSettings->initialSize * sizeof(void*));

        ALLOC_CHECK(object->expandable, ERROR);

        object->availableSize = initialSettings->initialSize;

        if(initialSettings->expandableConstant != 0)
        {
            object->expandableConstant = initialSettings->expandableConstant;
        }else
        {
            Log_e(TAG, "Expandable constant cannot be 0");
            return ERROR;
        }
        

    }else
    {
        Log_i(TAG, "Initial settings is NULL, restoring defaults");
        object->expandable = malloc(INITIAL_SIZE_DEFAULT * sizeof(void*));

        ALLOC_CHECK(object->expandable, ERROR);

        object->availableSize = INITIAL_SIZE_DEFAULT;
    }
    
    
    return SUCCESS;
    
}


/**
 * @brief Public method for handling new data pointer adding to existing list
 * 
 * @param object[in / out] pointer to vector structure  
 * @param dataObject[in]   pointer to new data structure
 */
bool Vector_append(VectorHandler_t object, const void* dataObject)
{
    NULL_GUARD(object, ERROR, VECTOR_NULL_PRINT);

    if(object->availableSize <= 0)
    {
        size_t newSize;

        Log_d(TAG, "Reallocating vector");

        newSize = (size_t) object->currentSize + (object->currentSize * object->expandableConstant);
        if(newSize == object->currentSize)
        {
            newSize += INITIAL_SIZE_DEFAULT;
        }
        object->expandable = realloc(object->expandable, newSize);

        ALLOC_CHECK(object->expandable, ERROR);
        
        object->availableSize = (newSize - object->currentSize);

    }else
    {
        object->expandable[object->currentSize] = dataObject;
        object->currentSize++;
        object->availableSize--;
    }
    
}

/**
 * @brief Public method for handling shrink to actual size
 * 
 * @param object[in / out] pointer to vector structure 
 */
void Vector_fit(VectorHandler_t object)
{
    NULL_GUARD(object,, VECTOR_NULL_PRINT);
    object->expandable = realloc(object->expandable, object->currentSize);
}


/**
 * @brief Public method for destroying vector object(deallocating resources)
 * 
 * @param object[in / out] pointer to vector structure 
 * @return Success state
 */
bool Vector_destroy(VectorHandler_t object)
{
    size_t idx;
    NULL_GUARD(object, ERROR, VECTOR_NULL_PRINT);

    if(object->expandable == NULL)
    {
        Log_e(TAG, "Passed wrong pointer for vector destruction");
        return ERROR;
    }

    
    for(idx = 0; idx < object->currentSize; idx++)
    {
        if(object->expandable[idx] != NULL)
        {
            free(object->expandable[idx]);
        }else
        {
            Log_w(TAG, "Vector allocation number:%d cannot be allocated due NULL", idx);
        }
        
    }

    free(object->expandable);
    object->availableSize =        0;
    object->currentSize =          0;
    object->expandableConstant =   0.0f;

    return SUCCESS;
}


/**
 * @brief Public method for debugging vector object current settings
 * 
 * @param object[in / out] pointer to vector structure 
 */
void Vector_print(VectorHandler_t object)
{
    NULL_GUARD(object, , VECTOR_NULL_PRINT);

    Log_d(TAG, "{\n\tavailableSize: %d,\n\t currentSize: %d\n}", object->availableSize, object->currentSize);

}
