
/**
 * @file symbol_table.h
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

#ifndef UTILITY_SYMBOL_TABLE_SYMBOL_TABLE_H_
#define UTILITY_SYMBOL_TABLE_SYMBOL_TABLE_H_


#include "../hashmap/hashmap.h"

typedef struct
{
    Hashmap_t allSymbolCalls;
    Hashmap_t allClasses;
}SymbolTable_t;

typedef enum
{
    FUNCTION,
    VARIABLE
}SymbolType_t;

typedef struct
{
    SymbolType_t type;
    void* symbolData;
}Symbol_t;


typedef SymbolTable_t* SymbolTableHandle_t;
typedef Symbol_t* SymbolHandle_t;

#endif // UTILITY_SYMBOL_TABLE_SYMBOL_TABLE_H_
