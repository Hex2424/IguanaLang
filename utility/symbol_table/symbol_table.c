/**
 * @file symbol_table.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2023-02-26
 */

#include "symbol_table.h"
#include "../misc/safety_macros.h"
#include <string.h>
#include "../logger/logger.h"
#include "../parser/parser_utilities/compiler_messages.h"

////////////////////////////////
// DEFINES
#define INITIAL_CALLS_HASHMAP_SIZE          10
#define INITIAL_CLASSES_HASHMAP_SIZE        5
////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "SYMBOL_TABLE";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS
static int declarationsForEach_(void *key, int count, void* value, void *user);

////////////////////////////////
// IMPLEMENTATION


/**
 * @brief Public method for creating SymbolTableInstance
 * 
 * @param[in/out] symbolTable - Symbol table object for initialization
 * @return                    - Success state
 */
bool SymbolTable_create(SymbolTableHandle_t symbolTable)
{
    if(!Hashmap_new(&symbolTable->allSymbolCalls, INITIAL_CALLS_HASHMAP_SIZE));
    {
        Log_e(TAG, "Failed to create Declarations hashmap");
        return ERROR;
    }

    if(!Hashmap_new(&symbolTable->allClasses, INITIAL_CLASSES_HASHMAP_SIZE))
    {
        Log_e(TAG, "Failed to create Symbol calls hashmap");
        return ERROR;
    }

    return SUCCESS;
}


int_fast8_t SymbolTable_addNewDeclaration(SymbolTableHandle_t symbolTable, const SymbolType_t type, const char* symbolFullName, void* object)
{
    HashmapHandle_t symbolsHashmap;
    uint32_t classNameLength;
    // Expecting to get full symbol name as "ClassName_symbolName" ;

    classNameLength = strchr(symbolFullName, '_') - symbolFullName;

    if(Hashmap_find(&symbolTable->allClasses, symbolFullName, classNameLength))
    {
        // When value gets found setting it to value
        symbolsHashmap = symbolTable->allClasses.value;
    }else
    {
        ALLOC_CHECK(symbolsHashmap, sizeof(Hashmap_t), ERROR);
        // When not found it creates new hashmap which will contain all symbols of specific class
        if(!Hashmap_new(symbolsHashmap, 3))
        {
            Log_e(TAG, "Failed to create Class hashmap");
            return ERROR;
        }

        // Setting Symbols hashmap as one of value of Classes Hashmap
        bool result = (bool) Hashmap_add(&symbolTable->allClasses, symbolFullName, classNameLength);
        *(symbolTable->allClasses.value) = symbolsHashmap;

        if(result)
        {
            Log_e(TAG, "False positive Hashmap_find function for some reason");
            return ERROR;
        }
        
    }

    // Until now all hashmap of classes related stuff should be handled properly

    if(Hashmap_set(symbolsHashmap, symbolFullName + classNameLength + SIZE_STR("_"), object))
    {
        ; // pabaigt
        return SUCCESS;
    }
    // TODO change bool return to error returning

    return SUCCESS;
}


int_fast8_t SymbolTable_addNewDeclarationRequest(SymbolTableHandle_t symbolTable, const SymbolType_t type, const char* className, const char* symbolName, void* object)
{
 
 
    return SUCCESS;
}


bool SymbolTable_destroy(SymbolTableHandle_t symbolTable)
{
    Hashmap_delete(&symbolTable->allSymbolCalls);
    
    Hashmap_forEach(&symbolTable->allClasses, declarationsForEach_, NULL);
    Hashmap_delete(&symbolTable->allClasses);
 
    return SUCCESS;
}


static int declarationsForEach_(void *key, int count, void* value, void *user)
{
    if(value != NULL)
    {
        Hashmap_delete((HashmapHandle_t) value);
    }


    return SUCCESS;
}

