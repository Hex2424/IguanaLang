/**
 * @file import_object.c
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
#include "import_object.h"
#include <stdio.h>
#include "../../../misc/safety_macros.h"
#include "../../../global_config/global_config.h"
#include "string.h"
#include "../../../file_manip/files.h"

////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "IMPORT_OBJECT";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION


/**
 * @brief Public method for getting Object name and generating random Object ID with file existance checks
 * 
 * @param[in/out] importObject - Import object where method result will be filled
 * @return bool                - Success state  
 */
bool ImportObject_generateRandomIDForObject(ImportObjectHandle_t importObject)
{
    FILE* tempDescriptor;
    char filePathForCheck[CFILES_LENGTH];
    
    // Putting filename as object name due Iguana principles
    importObject->objectNamePointerLength = Files_filepathGetFilename(importObject->name, &importObject->objectNamePointer);

    memcpy(filePathForCheck, TEMP_PATH, sizeof(TEMP_PATH) - 1);

    for(uint8_t retries = 0; retries < MAX_RETRIES_ID; retries++)
    {
        if(importObject == NULL)
        {
            Log_e(TAG, "Import object passed null for ID generation");
            return ERROR;
        }

        if(importObject->objectId.id == NULL)
        {
            Log_e(TAG, "Import object ID is null");
            return ERROR;
        }

        for(uint8_t hashIdx = 0; hashIdx < OBJECT_ID_LENGTH; hashIdx++)
        {
            importObject->objectId.id[hashIdx] = 'a' + Random_fast_rand() % 26;  // generating random byte
        }
        // char* slashPointer;
        // slashPointer = strrchr(importObject->name, '/');
        // if(slashPointer == NULL)
        // {
        //     slashPointer = importObject->name;
        // }
        // strcpy(importObject->objectId.id, slashPointer + 1);
        importObject->objectId.id[OBJECT_ID_LENGTH] = '\0';
        memcpy(filePathForCheck + (sizeof(TEMP_PATH) - 1), importObject->objectId.id, OBJECT_ID_LENGTH);

        filePathForCheck[CFILES_LENGTH - 2] = 'c';
        filePathForCheck[CFILES_LENGTH - 3] = '.';

        tempDescriptor = fopen(filePathForCheck, "r");
        if(tempDescriptor != NULL)
        {
            fclose(tempDescriptor);
            continue;
        }

        filePathForCheck[CFILES_LENGTH - 2] = 'h';

        tempDescriptor = fopen(filePathForCheck, "r");

        if(tempDescriptor != NULL)
        {
            fclose(tempDescriptor);
            continue;
        }

        return SUCCESS;

    }

    Log_w(TAG, "Something wrong with ID generator or all IDS was used");
    return ERROR;
}