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


/**
 * @brief Public method for seperating code into identifying tokens
 * 
 * @param codeString[in]    code buffer
 * @param length[in]        code buffer length
 * @param vector[out]       vector pointer for filling tokens
 * @return Success state
 */
bool Separator_getSeparatedWords(const char *codeString, const size_t length, VectorHandler_t vector)
{
    size_t tokenCount;
    InitialSettings_t settings;
    settings.initialSize = length / 2;
    settings.expandableConstant = (1.0f / 3.0f);
    settings.containsVectors = false; 

    if(!Vector_create(vector, &settings))
    {
        Log_e(TAG, "Failed to create vector object");
        return ERROR;
    }

    tokenCount = tokenize_(codeString, codeString + length, vector);
    Log_i(TAG, "Token Count: %d", (int)tokenCount);

    return SUCCESS;
}


/**
 * @brief Public method for converting buffer to corresponding tokens vector list
 * 
 * @param[in] begginingIterator start iterator of buffer
 * @param[in] maxIterator       end iterator of buffer
 * @param[out] vectorHandle     pointer to tokens vector object
 * @return token count
 */
static const size_t tokenize_(const char *begginingIterator, const char *maxIterator, VectorHandler_t vectorHandle)
{
    char *currentIterator; // TODO: if possible push to register
    char *wordIterator;
    bool breakTag;
    int existWordBuild;
    size_t overallLength;
    size_t tokenCount;

    wordIterator = begginingIterator;
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
            *currentIterator == ' '  ||
            *currentIterator == '\n' ||
            *currentIterator == '\r' ||
            *currentIterator == '\t')
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
                token = Tokenizer_wordToCorrespondingToken(wordIterator, existWordBuild);

                if(token == NULL)
                {
                    Log_e(TAG, "token is null");
                }

                Log_d(TAG, "token: %d %s", token->tokenType, token->valueString);
                Vector_append(vectorHandle, token);
                // Vector_print(vectorHandle);
                existWordBuild = 0;
                wordIterator = currentIterator;
            }
        }

        if (currentIterator < maxIterator)
        {
            existWordBuild++;
        }
    }

    return tokenCount;
}
