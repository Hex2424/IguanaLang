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


TokenHandler_t Tokenizer_wordToCorrespondingToken(const char *seperation, const size_t length)
{
    int bindingLinePos;
    TokenHandler_t tokenHandler;
    

    if(seperation == NULL)
    {
        Log_e(TAG, "Got NULL Seperation Object for parsing %d", 5);
        return NULL;
    }

    tokenHandler = malloc(sizeof(Token_t)); // dynamic allocation, must deallocate afterwards

    if(tokenHandler == NULL)
    {
        Log_e(TAG, "Failed to allocate token object, heap issue %d", 5);
        return NULL;
    }


    for(bindingLinePos = 0; bindingLinePos < (sizeof(bindingsTable_) / sizeof(BindingType_t)); bindingLinePos++)
    {
        if(strcmp(seperation, bindingsTable_[bindingLinePos].expression) == 0)
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

static bool handleUnknownType_(TokenHandler_t tokenHandle, const char* expression, const size_t expressionSize)
{
    tokenHandle->tokenType = NAMING;
    
    tokenHandle->valueString = malloc(expressionSize);

    if(tokenHandle->valueString == NULL)
    {
        Log_e(TAG, "Failed to allocare value string, heap issue");
        return false;
    }

    memcpy(tokenHandle->valueString, expression, expressionSize);

    return true;
}