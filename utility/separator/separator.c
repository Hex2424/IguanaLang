#include "separator.h"
#include <string.h>
#include <stdio.h>
////////////////////////////////
// DEFINES

typedef uint64_t MAX_TOKENS;
typedef uint64_t MAX_CODE_LENGTH;

////////////////////////////////
// PRIVATE TYPES

static const char allowedNamingSymbols_[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM0123456789_";

////////////////////////////////
// PRIVATE METHODS

static const size_t calculateTokens_(const char* begginingIterator, const char* maxIterator);




void Separator_getSeparatedWords(const char* codeString, const size_t length)
{
    size_t tokenCount;
    tokenCount = calculateTokens_(codeString, codeString + length);
    printf("%d", (int)tokenCount);
}


/**
 * @brief Private method for calculating how many tokens exist in code string
 * 
 * @param[in] codeString
 * 
 * @return MAX_TOKENS 
 */
static const size_t calculateTokens_(const char* begginingIterator, const char* maxIterator)
{
    char* currentIterator; //TODO: if possible push to register
    bool breakTag;
    int existWordBuild;
    size_t overallLength;
    size_t tokenCount;

    // overallLength = maxIterator - begginingIterator;
    existWordBuild = 0;
    tokenCount = 0;

    // iterating code characters via Iterators for faster performance
    for(currentIterator = begginingIterator; currentIterator < maxIterator; currentIterator++)
    {
        bool symbolExists;
        bool prevSymbolExists;

        breakTag = false;

        symbolExists = memchr(allowedNamingSymbols_, *currentIterator, sizeof(allowedNamingSymbols_)) != NULL;
        if(currentIterator != begginingIterator)
        {
            prevSymbolExists = memchr(allowedNamingSymbols_, *(currentIterator - 1), sizeof(allowedNamingSymbols_)) != NULL;
        }else
        {
            prevSymbolExists = false;
        }
     
        if(!symbolExists || (!prevSymbolExists && symbolExists))
        {
            breakTag = true;
        }

        while(
            *currentIterator == ' '     || 
            *currentIterator == '\n'    ||
            *currentIterator == '\r')
            {
                breakTag = true;
                currentIterator++;
                if(currentIterator >= maxIterator)
                {
                    break;
                }
            }
        if(breakTag)
        {
            if(existWordBuild != 0)
            {
                tokenCount++;
                existWordBuild = 0;
            }
        }

        if(currentIterator < maxIterator)
        {
            existWordBuild++;
        }
    }

    return tokenCount;
}
