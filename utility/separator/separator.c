/**
 * @file separator.c
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


#include "separator.h"
#include "../tokenizer/tokenizer.h"
#include "../vector/vector.h"
#include "../logger/logger.h"
#include <string.h>
#include <stdio.h>

////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS

static const char* TAG = "SEPARATOR";
typedef uint64_t MAX_TOKENS;
typedef uint64_t MAX_CODE_LENGTH;

////////////////////////////////
// PRIVATE TYPES

static const char allowedNamingSymbols_[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM0123456789_";

////////////////////////////////
// PRIVATE METHODS

static const size_t calculateTokens_(const char *begginingIterator, const char *maxIterator);
static const size_t tokenize_(const char *begginingIterator, const char *maxIterator, VectorHandler_t vectorHandle);

////////////////////////////////
// IMPLEMENTATION



bool Separator_getSeparatedWords(const char *codeString, const size_t length)
{
    size_t tokenCount;
    Vector_t vector;
    InitialSettings_t settings;
    settings.initialSize = length / 2;
    settings.expandableConstant = (1.0f / 3.0f);

    if(!Vector_create(&vector, &settings))
    {
        Log_e(TAG, "Failed to create vector object");
        return ERROR;
    }

    tokenCount = tokenize_(codeString, codeString + length, &vector);
    printf("%d", (int)tokenCount);
    return SUCCESS;
}


static const size_t tokenize_(const char *begginingIterator, const char *maxIterator, VectorHandler_t vectorHandle)
{
    char *currentIterator; // TODO: if possible push to register
    bool breakTag;
    int existWordBuild;
    size_t overallLength;
    size_t tokenCount;

    overallLength = maxIterator - begginingIterator;
    existWordBuild = 0;
    tokenCount = 0;

    // iterating code characters via Iterators for faster performance
    for (currentIterator = (char*) begginingIterator; currentIterator < maxIterator; currentIterator++)
    {
        bool symbolExists;
        bool prevSymbolExists;

        breakTag = false;

        symbolExists = memchr(allowedNamingSymbols_, *currentIterator, sizeof(allowedNamingSymbols_)) != NULL;
        if (currentIterator != begginingIterator)
        {
            prevSymbolExists = memchr(allowedNamingSymbols_, *(currentIterator - 1), sizeof(allowedNamingSymbols_)) != NULL;
        }
        else
        {
            prevSymbolExists = false;
        }

        if (!symbolExists || (!prevSymbolExists && symbolExists))
        {
            breakTag = true;
        }

        while (
            *currentIterator == ' ' ||
            *currentIterator == '\n' ||
            *currentIterator == '\r')
        {
            breakTag = true;
            currentIterator++;
            if (currentIterator >= maxIterator)
            {
                break;
            }
        }
        if (breakTag)
        {
            if (existWordBuild != 0)
            {
                tokenCount++;

                TokenHandler_t token;
                token = Tokenizer_wordToCorrespondingToken(currentIterator - existWordBuild, existWordBuild);

                if(token == NULL)
                {
                    Log_e(TAG, "token is null");
                }

                Log_i(TAG, "token: %d %s", token->tokenType, token->valueString);
                Vector_append(vectorHandle, token);
                // Vector_print(vectorHandle);
                existWordBuild = 0;
            }
        }

        if (currentIterator < maxIterator)
        {
            existWordBuild++;
        }
    }

    return tokenCount;
}
