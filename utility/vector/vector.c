/**
 * @file vector.c
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
#include <vector.h>
#include <logger.h>


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
 * @param[out] object           pointer to vector structure
 * @param[in] initialSettings   initial advanced vector object settings for manual setup
 * 
 * @return                      Success state
 */

bool Vector_create(VectorHandler_t object, const InitialSettingsHandler_t initialSettings)
{
    NULL_GUARD(object, ERROR, VECTOR_NULL_PRINT);

    object->currentSize = 0;
    object->containsVectors = false;

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
        ALLOC_CHECK(object->expandable, initialSettings->initialSize * sizeof(void*), ERROR);

        object->availableSize = initialSettings->initialSize;
        object->containsVectors = initialSettings->containsVectors;

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
        object->expandableConstant = EXPANDABLE_CONSTANT_DEFAULT; // default expandable constant
        ALLOC_CHECK(object->expandable, INITIAL_SIZE_DEFAULT * sizeof(void*), ERROR);

        object->availableSize = INITIAL_SIZE_DEFAULT;
    }
    
    
    return SUCCESS;
    
}


/**
 * @brief Public method for handling new data pointer adding to existing list
 * 
 * @param[out] object       pointer to vector structure  
 * @param[in] dataObject    pointer to new data structure
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

        REALLOC_CHECK(object->expandable, newSize * sizeof(void*), ERROR);
        
        object->availableSize = (newSize - object->currentSize);

    }

    object->expandable[object->currentSize] = (void*) dataObject;
    object->currentSize++;
    object->availableSize--;

    return SUCCESS;
}

/**
 * @brief Public method for handling shrink to actual size
 * 
 * @param[out] object pointer to vector structure 
 */
bool Vector_fit(VectorHandler_t object)
{
    NULL_GUARD(object, ERROR, VECTOR_NULL_PRINT);
    REALLOC_CHECK(object->expandable,object->currentSize * sizeof(void*), ERROR);
    object->availableSize = 0;

    return SUCCESS;
}


/**
 * @brief Public method for destroying vector object(deallocating resources)
 * 
 * @param[out] object        pointer to vector structure
 * 
 * @return                   Success state
 */
bool Vector_destroy(VectorHandler_t vector)
{
    size_t idx;
    NULL_GUARD(vector, ERROR, VECTOR_NULL_PRINT);

    if(vector->expandable == NULL)
    {
        Log_e(TAG, "Passed wrong pointer for vector destruction");
        return ERROR;
    }
    
    for(idx = 0; idx < vector->currentSize; idx++)
    {
        void* object = (void*) vector->expandable[idx];

        if(object != NULL)
        {

            if(vector->containsVectors)
            {
                Vector_destroy((VectorHandler_t) object);
            }
            free(vector->expandable[idx]);
        }else
        {
            Log_w(TAG, "Vector allocation number:%d cannot be allocated due NULL", idx);
        }
        
    }

    free(vector->expandable);

    vector->availableSize =        0;
    vector->currentSize =          0;
    vector->expandableConstant =   0.0f;

    return SUCCESS;
}


/**
 * @brief Public method for debugging vector object current settings
 * 
 * @param[out] object pointer to vector structure 
 */
void Vector_print(VectorHandler_t object)
{
    NULL_GUARD(object, , VECTOR_NULL_PRINT);

    Log_d(TAG, "{\n\tavailableSize: %d,\n\t currentSize: %d\n}", object->availableSize, object->currentSize);

}
