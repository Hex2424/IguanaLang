#include "bitfit.h"
#include <stdlib.h>
#include "../../structures/variable/variable.h"
#include <arch_specific.h>
#include <logger.h>

static const char* TAG = "BITFIT";

#define MAX_GROUPS_POSSIBLE         1024

typedef bool (*fitAssignFunction_t)(VectorHandler_t vector, const uint8_t groupSizeMax, BitpackSize_t* sizeNeededForVariables);

static bool firstFitMethodFunction_(VectorHandler_t vector, const uint8_t groupSizeMax, BitpackSize_t* sizeNeededForVariables);

static int variableIteratorCallback_(void *key, int count, void* value, void *user);

int comp (const void* elem1, const void* elem2);


bool Bitfit_assignGroupsAndPositionForVariableHashmap_(const HashmapHandle_t variablesHashmap, const BitFitMethod_t fitType, BitpackSize_t* sizeNeededForVariables)
{
    Vector_t vector;
    InitialSettings_t settingVector;

    settingVector.containsVectors = false;
    settingVector.expandableConstant = EXPANDABLE_CONSTANT_DEFAULT;
    settingVector.initialSize = variablesHashmap->length;

    if(!Vector_create(&vector, &(settingVector)))
    {
        Log_e(TAG, "Failed to create vector for variable fitpack");
        return ERROR;
    }

    // Put links to variables from hashmap to vector
    Hashmap_forEach(variablesHashmap, variableIteratorCallback_, &vector);

    return Bitfit_assignGroupsAndPositionForVariableVector_(&vector, ARCHITECTURE_DEFAULT_BITS, sizeNeededForVariables);
}


bool Bitfit_assignGroupsAndPositionForVariableVector_(const VectorHandler_t variablesVector, const BitFitMethod_t fitType, BitpackSize_t* sizeNeededForVariables)
{
    fitAssignFunction_t bitFitFunction;

    switch (fitType)
    {
        case FIRST_FIT: bitFitFunction = firstFitMethodFunction_; break;

        // Other fits not supported yet
        default: return ERROR;
    }

    return bitFitFunction(variablesVector, ARCHITECTURE_DEFAULT_BITS, sizeNeededForVariables);
}

static bool firstFitMethodFunction_(VectorHandler_t vector, const uint8_t groupSizeMax, BitpackSize_t* sizeNeededForVariables)
{
    // TODO: change this number later
    uint32_t used[MAX_GROUPS_POSSIBLE] = {0};

    uint32_t highestGroupIdxReached = 0;

    // Sort fields by bitsize (largest first)
    qsort (vector->expandable, vector->currentSize, sizeof(VariableObjectHandle_t), comp);

    for (uint32_t varIdx = 0; varIdx < vector->currentSize; ++varIdx) {

        VariableObjectHandle_t variable;
        bool placed;

        variable = (VariableObjectHandle_t) vector->expandable[varIdx];
        placed = false;

        for (int groupIdx = 0; groupIdx < MAX_GROUPS_POSSIBLE; ++groupIdx) 
        {
            if (used[groupIdx] + variable->bitpack <= groupSizeMax) 
            {
                variable->belongToGroup = groupIdx;
                variable->posBit = used[groupIdx];

                used[groupIdx] += variable->bitpack;

                placed = true;

                if(highestGroupIdxReached < groupIdx)
                {
                    highestGroupIdxReached = groupIdx;
                }

                break;
            }
        }

        if (!placed) 
        {
            if(variable->bitpack > groupSizeMax)
            {
                Log_e(TAG, "Error bigger than %u not supported now", groupSizeMax);
            }else
            {
                Log_e(TAG, "ERROR: Too much bit groups, bit:%llu(%s) %s doesnt fit\n", variable->bitpack, variable->castedFile, variable->objectName);
            }

            return ERROR;   
        }
    }

    *sizeNeededForVariables = (highestGroupIdxReached * groupSizeMax) + used[highestGroupIdxReached];
    
    return SUCCESS;
}


static int variableIteratorCallback_(void *key, int count, void* value, void *user)
{
    return Vector_append((VectorHandler_t) user, value);
}


int comp (const void * elem1, const void * elem2) 
{
    VariableObjectHandle_t a = (VariableObjectHandle_t) elem1;
    VariableObjectHandle_t b = (VariableObjectHandle_t) elem2;
    
    return (b->bitpack - a->bitpack);

    if (a->bitpack < b->bitpack)
    {
        return 1;
    }

    if (a->bitpack > b->bitpack)
    {
        return -1;
    }

    return strcmp(a->objectName, b->objectName);

}
