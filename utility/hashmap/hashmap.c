/**
 * @file hashmap.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-11-06
 */
#include "hashmap.h"
#include "string.h"

////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "HASHMAP";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS
static uint16_t calculateStringChecksum_(const char* keyString);
static HashmapPairHandle_t findEntry_(const HashmapHandle_t hashmap, const uint16_t keySum, const char* realKey);

////////////////////////////////
// IMPLEMENTATION


/**
 * @brief Public method for initializing hashmap
 * 
 * @param[in/out] hashmap   - Hashmap object for initialization
 * @return                  - Success state
 */
bool Hashmap_create(HashmapHandle_t hashmap)
{
    InitialSettings_t settings;
    settings.initialSize = 7;
    settings.containsVectors = false;

    if(!Vector_create(&hashmap->entries, &settings))
    {
        Log_e(TAG, "Hashmap creating got failed due vector");
        return ERROR;
    }
    return SUCCESS;
}


/**
 * @brief Public method for putting new entry or replacing existing with specific string key
 * 
 * @param[in/out] hashmap  - hashmap object for manipulation
 * @param[in] keyString    - Key of entry to put
 * @param[in] object       - Object pointer to be put inside hashmap 
 * @return                 - Success state
 */
bool Hashmap_putEntry(HashmapHandle_t hashmap, const char* keyString, const void* object)
{
    HashmapPairHandle_t pair;
    uint16_t currentKeySum;
    currentKeySum = calculateStringChecksum_(pair->realKey);

    pair = findEntry_(hashmap, currentKeySum, keyString);

    if(pair == NULL)
    {
        pair = malloc(sizeof(HashmapPair_t));
        ALLOC_CHECK(pair, ERROR);

        if(!Vector_append(&hashmap->entries, pair))
        {
            Log_e(TAG, "Failed to add hashmap entry");
            return ERROR;
        }
    }
    pair->keySum = currentKeySum;
    pair->realKey = keyString;
    pair->object = object;

    return SUCCESS;
}


/**
 * @brief Public method for getting specific entry with key
 * 
 * @param[in] hashmap    - Hashmap object
 * @param[in] keyString  - Key for object retrieval
 * @return               - Success state 
 */
bool Hashmap_getEntry(const HashmapHandle_t hashmap, const char* keyString)
{
    return findEntry_(hashmap, calculateStringChecksum_(keyString), keyString);
}


/**
 * @brief Private method for finding entry by StringKey checksum
 * 
 * @param[in] hashmap - Hashmap object
 * @param[in] keySum  - Keysum against which finding entry 
 * @return            - HashmapPairHandle_t as entry, NULL - no entry found 
 */
static HashmapPairHandle_t findEntry_(const HashmapHandle_t hashmap, const uint16_t keySum, const char* key)
{
    HashmapPairHandle_t currentPair;

    for(uint64_t hashmapEntryIdx = 0; hashmapEntryIdx < hashmap->entries.currentSize; hashmapEntryIdx++)
    {
        currentPair = ((HashmapPairHandle_t)hashmap->entries.expandable[hashmapEntryIdx]);
        if(currentPair->keySum == keySum && (strcmp(currentPair->realKey, key) == 0))
        {
            return currentPair;
        }
    } 
    return NULL;
}

/**
 * @brief Private method for calculating "Hash of string key", in this case checksum
 * 
 * @param[in] keyString - key provides as string
 * @return              - checksum of string bytes
 */
static uint16_t calculateStringChecksum_(const char* keyString)
{
    if(keyString == NULL)
    {
        return 0;
    }

    uint64_t keySize = strlen(keyString);
    uint16_t checksum = 0;
    for(uint64_t keyCharIdx = 0; keyCharIdx < keySize; keyCharIdx++)
    {
        checksum += keyString[keyCharIdx];
    }
    return checksum;
}

bool Hashmap_destroy(HashmapHandle_t hashmap)
{
    if(!Vector_destroy(&hashmap->entries))
    {
        Log_e(TAG, "Hashmap destruction got failed");
        return ERROR;
    }
    return SUCCESS;
}