/**
 * @file separator.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-05
 */


#include "separator.h"
#include "../tokenizer/tokenizer.h"
#include <vector.h>
#include <logger.h>
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

static const size_t tokenize_(const char *begginingIterator, const char *maxIterator, VectorHandler_t vectorHandle,const char* currentFile);
static TokenHandler_t createEndFileToken_(void);
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
bool Separator_getSeparatedWords(const char *codeString, const size_t length, VectorHandler_t vector, const char* filePath)
{
    size_t tokenCount;
    InitialSettings_t settings;
    settings.initialSize = length / 2;
    settings.expandableConstant = EXPANDABLE_CONSTANT_DEFAULT;
    settings.containsVectors = false; 

    TokenHandler_t endFileToken = createEndFileToken_();
    
    if(!Vector_create(vector, &settings))
    {
        Log_e(TAG, "Failed to create vector object");
        return ERROR;
    }

    tokenCount = tokenize_(codeString, codeString + length, vector, filePath);
    Log_i(TAG, "Token Count: %d", (int)tokenCount);


    
    if(!Vector_append(vector, endFileToken))
    {
        Log_e(TAG, "Failed to END_FILE token");
        return ERROR;
    }

    return SUCCESS;
}


static TokenHandler_t createEndFileToken_(void)
{
    TokenHandler_t endFileToken;

    ALLOC_CHECK(endFileToken, sizeof(Token_t), NULL);

    endFileToken->tokenType = END_FILE;
    endFileToken->location.column = 0;
    endFileToken->location.line = 0;
    endFileToken->valueString = NULL;

    return endFileToken;

}

/**
 * @brief Public method for converting buffer to corresponding tokens vector list
 * 
 * @param[in] begginingIterator start iterator of buffer
 * @param[in] maxIterator       end iterator of buffer
 * @param[out] vectorHandle     pointer to tokens vector object
 * @return token count
 */
static const size_t tokenize_(const char *begginingIterator, const char *maxIterator, VectorHandler_t vectorHandle,const char* currentFile)
{
    char *currentIterator; // TODO: if possible push to register
    char *wordIterator;
    bool breakTag;
    bool isLiteral;
    uint32_t existWordBuild;
    size_t tokenCount;

    size_t currentLine = 1;
    size_t currentColumn = 1;

    size_t lastLine = 1;
    size_t lastColumn = 1;

    isLiteral = false;
    wordIterator = (char*) begginingIterator;
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
            currentColumn++;
            
            switch (*currentIterator)
            {
                case '\n':
                {
                    currentLine++;
                    currentColumn = 1;
                    isLiteral = false;
                }break;

                case '\r':
                {
                    currentColumn = 1;
                }break;
            }

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
                TokenHandler_t token;
                uint32_t sizeOfSeperation = existWordBuild;

                if(!isLiteral)
                {
                    tokenCount++;

                    while (
                        *wordIterator == ' '  ||
                        *wordIterator == '\n' ||
                        *wordIterator == '\r' ||
                        *wordIterator == '\t')
                    {
                        wordIterator++;
                        existWordBuild--;
                    }


                    token = Tokenizer_wordToCorrespondingToken(wordIterator, sizeOfSeperation);

                    token->location.column = lastColumn;     // setting up file location settings for debugging errors
                    token->location.line = lastLine;
                    token->location.filename = (char*) currentFile;


                    lastColumn = currentColumn;
                    lastLine = currentLine;

                    if(token == NULL)
                    {
                        Log_e(TAG, "token is null");
                    }

                    Log_d(TAG, "token: %d %s", token->tokenType, token->valueString);

                    if(!Vector_append(vectorHandle, token))
                    {
                        Log_e(TAG, "Failed to append tokenType:%d", token->tokenType);
                        return ERROR;
                    }
                    // Vector_print(vectorHandle);
                    existWordBuild = 0;
                    wordIterator = currentIterator;
                }

            }
        }

        if (currentIterator < maxIterator)
        {
            existWordBuild++;
        }

        if(*currentIterator == '\"' || *currentIterator == '\'')
        {
            isLiteral = !isLiteral;
        }

        currentColumn++;
    }

    if(existWordBuild != 0)
    {
        TokenHandler_t token;
        if(!isLiteral)
        {
            tokenCount++;
            token = Tokenizer_wordToCorrespondingToken(wordIterator, existWordBuild);

            token->location.column = lastColumn;     // setting up file location settings for debugging errors
            token->location.line = lastLine;
            token->location.filename = (char*) currentFile;


            lastColumn = currentColumn;
            lastLine = currentLine;

            if(token == NULL)
            {
                Log_e(TAG, "token is null");
            }

            Log_d(TAG, "token: %d %s", token->tokenType, token->valueString);

            if(!Vector_append(vectorHandle, token))
            {
                Log_e(TAG, "Failed to append tokenType:%d", token->tokenType);
                return ERROR;
            }
        }
    }
    return tokenCount;
}

