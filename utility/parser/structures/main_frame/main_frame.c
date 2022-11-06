/**
 * @file main_frame.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-15
 */
#include "main_frame.h"
#include "../../../vector/vector.h"
#include "../../../logger/logger.h"
////////////////////////////////
// DEFINES
#define NAME_VARIABLES_VECTOR   "variables"
#define NAME_IMPORTS_VECTOR     "imports"
#define NAME_METHODS_VECTOR     "methods"

#define FAILED_CREATION_LOG     "Could not create %s hashmap"
#define FAILED_DESTROY_LOG      "Could not destroy %s hashmap"
////////////////////////////////
// PRIVATE CONSTANTS


////////////////////////////////
// PRIVATE TYPES
static const char* TAG = "MAIN_FRAME";

////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION


/**
 * @brief Public method for initializing mainFrame object
 * 
 * @param[out] handle   mainFrame object pointer
 * 
 * @return              Success state
 */
bool MainFrame_init(MainFrameHandle_t handle)
{
    InitialSettings_t settings;

    settings.initialSize = 5;
    settings.expandableConstant = 1.0f / 3.0f;
    settings.containsVectors = true;

    if(!Hashmap_create(&handle->imports, &settings))
    {
        Log_e(TAG, FAILED_CREATION_LOG, NAME_IMPORTS_VECTOR);
        return ERROR;
    }

    if(!Hashmap_create(&handle->classVariables, &settings))
    {
        Log_e(TAG, FAILED_CREATION_LOG, NAME_VARIABLES_VECTOR);
        return ERROR;
    }

    if(!Hashmap_create(&handle->methods, &settings))
    {
        Log_e(TAG, FAILED_CREATION_LOG, NAME_METHODS_VECTOR);
        return ERROR;
    }

    return SUCCESS;

}

/**
 * @brief Public method for completely destroying contents of maindFrame object
 * 
 * @param[out] handle       object pointer to mainFrame
 * 
 * @return                  Success state
 */
bool MainFrame_destroy(MainFrameHandle_t handle)
{

    if(!Hashmap_destroy(&handle->imports))
    {
        Log_e(TAG, FAILED_DESTROY_LOG, NAME_IMPORTS_VECTOR);
        return ERROR;
    }

    if(!Hashmap_destroy(&handle->classVariables))
    {
        Log_e(TAG, FAILED_DESTROY_LOG, NAME_VARIABLES_VECTOR);
        return ERROR;
    }

    if(!Hashmap_destroy(&handle->methods))
    {
        Log_e(TAG, FAILED_DESTROY_LOG, NAME_METHODS_VECTOR);
        return ERROR;
    }


    return SUCCESS;

}