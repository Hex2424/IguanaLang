/**
 * @file tokenizer.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-05
 */

#include "tokenizer.h"
#include "token/token_database/token_bindings.h"
#include <string.h>
#include <stdio.h>
#include "../logger/logger.h"
#include "../misc/safety_macros.h"

////////////////////////////////
// DEFINES

////////////////////////////////
// PRIVATE TYPES

static const char* TAG = "TOKENIZER";

////////////////////////////////
// PRIVATE METHODS

static bool handleUnknownType_(TokenHandler_t tokenHandle, const char* expression, const size_t expressionSize);

////////////////////////////////
// IMPLEMENTATION

/**
 * @brief Public method for searching token type for specific string
 * 
 * @param[in] seperation string 
 * @param[in] length     string size
 * @return dynamically allocated token object 
 */
TokenHandler_t Tokenizer_wordToCorrespondingToken(const char *seperation, const size_t length)
{
    int bindingLinePos;
    size_t bindingTableCount;
    TokenHandler_t tokenHandler;
    
    NULL_GUARD(seperation, NULL, Log_e(TAG, "Got NULL Seperation Object for parsing %d", 5));

    tokenHandler = malloc(sizeof(Token_t)); // dynamic allocation, must deallocate afterwards

    ALLOC_CHECK(tokenHandler, NULL);

    bindingTableCount = (sizeof(bindingsTable_) / sizeof(BindingType_t));

    for(bindingLinePos = 0; bindingLinePos < bindingTableCount; bindingLinePos++)
    {
        if(memcmp(seperation, bindingsTable_[bindingLinePos].expression, length) == 0)
        {

            tokenHandler->tokenType = bindingsTable_[bindingLinePos].type;
            tokenHandler->valueString = NULL;
            return tokenHandler;
            // Litterals matching, can proceed to Token creation

        }

        // nothing
    }

    if(!handleUnknownType_(tokenHandler, seperation, length))
    {
        Log_e(TAG, "Failed to tokenize expression (%s)", seperation);
        return NULL;
    }


    return tokenHandler;
}

/**
 * @brief Public method for handling NAMING type
 * 
 * @param[out] tokenHandle      pointer to Token object
 * @param[in] expression        string
 * @param[in] expressionSize    string size
 * @return Success type 
 */
static bool handleUnknownType_(TokenHandler_t tokenHandle, const char* expression, const size_t expressionSize)
{
    tokenHandle->tokenType = NAMING;
    
    tokenHandle->valueString = malloc(expressionSize);

    ALLOC_CHECK(tokenHandle->valueString, false);

    memcpy(tokenHandle->valueString, expression, expressionSize);
    
    return true;
}