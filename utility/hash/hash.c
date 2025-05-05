/**
 * @file hash.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-16
 */

#include "hash.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include "random/random.h"
////////////////////////////////
// DEFINES
#define ERROR           0
#define SUCCESS         1


////////////////////////////////
// PRIVATE CONSTANTS
static const char hexAlphabet[] = "abcdef0123456789";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS

static uint32_t calculateInputChecksum_(const char* input, const uint32_t inputLength);
static bool generateInputSeededInitialHash_(const char* input, const uint32_t inputLength, char* output, const uint32_t outputSize);
static bool apply1BitSlidingAlgorithm_(const char* input, const uint32_t inputLength, char* output, const uint32_t outputSize);
static bool formatOutputAsHexadecimal_(char* output, const uint32_t outputSize);

////////////////////////////////
// IMPLEMENTATION


/**
 * @brief Public method for hashing using eHash algorithm
 * 
 * @param[in] text          input string for hashing
 * @param[out] output       output string for hashing
 * @param[in] outputSize    output size in bytes
 * @param[in] inputSize     input size in bytes
 * @return                  Success State 
 */
bool EHash_hash(const char* text, const uint32_t inputSize, char* output, const uint32_t outputSize)
{

    if(!generateInputSeededInitialHash_(text, inputSize,output, outputSize))
    {
        // failed to generate initial hash
        return ERROR;
    }

    if(!apply1BitSlidingAlgorithm_(text, inputSize, output, outputSize))
    {
        // failed to calculate via sliding bit method
        return ERROR;
    }

        // printf("%d\n", output[0]);

    if(!formatOutputAsHexadecimal_(output, outputSize))
    {
        return ERROR;
    }


    // On success
    return SUCCESS;
    
}


static bool apply1BitSlidingAlgorithm_(const char* input, const uint32_t inputLength, char* output, const uint32_t outputSize)
{
    uint8_t slidingMask;
    uint32_t inputIdx;
    uint32_t checksum;
    uint32_t maxSize;

    slidingMask = 1;
    inputIdx = 0;

    if(output == NULL || input == NULL || outputSize == 0 || inputLength == 0)
    {
        // something wrong with function input, may be heap leak
        return ERROR;
    }
    maxSize = max(outputSize, inputLength);


    for(uint32_t outputIdx = 0; outputIdx < maxSize; outputIdx++)
    {

        if(inputIdx >= inputLength)
        {
            inputIdx = 0;           // overlooping input byte idx
        }

        Random_fast_srand((int) input[inputIdx]);

        for(uint32_t outputIdx2nd = outputIdx; outputIdx2nd < outputSize; outputIdx2nd++)
        {
            output[outputIdx2nd] ^= Random_fast_rand() % 256;
        }

        inputIdx++;
    }

    return SUCCESS;
}

static bool formatOutputAsHexadecimal_(char* output, const uint32_t outputSize)
{
    for(uint32_t outputIdx = 0; outputIdx < outputSize; outputIdx++)
    {
        output[outputIdx] = (char) hexAlphabet[((uint8_t) output[outputIdx]) % (sizeof(hexAlphabet) - 1)];
    }
    return SUCCESS;
}

static bool generateInputSeededInitialHash_(const char* input, const uint32_t inputLength, char* output, const uint32_t outputSize)
{
    uint32_t inputIdx;

    inputIdx = 0;

    Random_fast_srand((int) calculateInputChecksum_(input, inputLength));     // placing input string bytes checksum as random seed

    for(uint32_t hashIdx = 0; hashIdx < outputSize; hashIdx++)
    {
        output[hashIdx] = Random_fast_rand() % 256;      // getting remainder of 256 for random byte value

        if(inputIdx > inputLength)
        {
            inputIdx = 0;
        }else
        {
            inputIdx++;
        }

    }

    return SUCCESS;
}


/**
 * @brief Public method for calculating all input bytes values sum
 * 
 * @param[in] input         input string
 * @param[in] inputLength   input string length
 * @return                  bytes checksum number
 */
static uint32_t calculateInputChecksum_(const char* input, const uint32_t inputLength)
{
    uint32_t checksum;
    checksum = 0;
    
    if(input == NULL || inputLength == 0)
    {
        // null input given
        printf("input is null or inputlength not given\n");
        return ERROR;
    }


    for(uint32_t inputIdx = 0; inputIdx < inputLength; inputIdx++)
    {
        checksum += input[inputIdx];
    }

    return checksum ^ 24;
}