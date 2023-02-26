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

////////////////////////////////
// DEFINES
#define INITIAL_CALLS_HASHMAP_SIZE          10
#define INITIAL_CLASSES_HASHMAP_SIZE        5
////////////////////////////////
// PRIVATE CONSTANTS


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
        return ERROR;
    }

    if(!Hashmap_new(&symbolTable->allClasses, INITIAL_CLASSES_HASHMAP_SIZE))
    {
        return ERROR;
    }

    return SUCCESS;
}


bool SymbolTable_addNewDeclaration(SymbolTableHandle_t symbolTable, const SymbolType_t type, const char* symbolFullName, void* object)
{
    HashmapHandle_t value;


    



    if(!Hashmap_find(&symbolTable->allClasses, className, ))
    {
        value = 
    }

    if(Hashmap_set(&symbolTable->allClasses, className))
    {

    }


    return SUCCESS;
}


bool SymbolTable_addNewDeclarationRequest(SymbolTableHandle_t symbolTable, const SymbolType_t type, const char* className, const char* symbolName, void* object)
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

