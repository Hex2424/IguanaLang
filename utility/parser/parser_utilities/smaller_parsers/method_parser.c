/**
 * @file method_parser.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-09
 */

#include "method_parsers.h"
#include "../global_parser_utility.h"
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "METHOD_PARSER";

////////////////////////////////
// PRIVATE TYPES


////////////////////////////////
// PRIVATE METHODS

static bool parseMethodParameters_(MethodObjectHandle_t methodHandle);

////////////////////////////////
// IMPLEMENTATION

inline bool MethodParser_parseMethod(MainFrameHandle_t root)
{
    MethodObjectHandle_t methodHandle;

    methodHandle = malloc(sizeof(MethodObject_t));
    ALLOC_CHECK(methodHandle, ERROR);

    if(!parseMethodParameters_(methodHandle))
    {

    }
}

static bool parseMethodParameters_(MethodObjectHandle_t methodHandle)
{

}