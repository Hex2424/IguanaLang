/**
 * @file hashmap.h
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

#ifndef UTILITY_HASHMAP_HASHMAP_H_
#define UTILITY_HASHMAP_HASHMAP_H_
#include <stdbool.h>
#include <stdint.h>
#include "../vector/vector.h"
#include "../logger/logger.h"

typedef struct
{
    void* object;
    char* realKey;
    uint16_t keySum;
}HashmapPair_t;

typedef HashmapPair_t* HashmapPairHandle_t;

typedef struct
{
    Vector_t entries;
}Hashmap_t;

typedef Hashmap_t* HashmapHandle_t; 


bool Hashmap_destroy(HashmapHandle_t hashmap);
bool Hashmap_create(HashmapHandle_t hashmap, InitialSettingsHandler_t settings);
bool Hashmap_putEntry(HashmapHandle_t hashmap, const char* keyString, const void* object);
bool Hashmap_getEntry(const HashmapHandle_t hashmap, const char* keyString);
void* Hashmap_at(const HashmapHandle_t hashmap, const uint32_t idx);
uint64_t Hashmap_size(const HashmapHandle_t hashmap);

#endif // UTILITY_HASHMAP_HASHMAP_H_