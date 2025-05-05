/**
 * @file parser.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-07
 */

#include "parser.h"
#include "../tokenizer/token/token.h"
#include "parser_utilities/global_parser_utility.h"
#include "parser_utilities/smaller_parsers/method_parser/method_parsers.h"
#include "parser_utilities/smaller_parsers/var_parser/var_parser.h"
#include "parser_utilities/post_parsing_utility/bitfit.h"

#include <string.h>
#include "../hash/random/random.h"

////////////////////////////////
// DEFINES

#define cTokenP                                     (*currentToken)
#define cTokenType                                  cTokenP -> tokenType
#define LONGEST_POSSIBLE_IGUANA_EXTENSION_LENGTH    sizeof("iguana")


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "PARSER";


// Ordering affects speed, the most common should be first element
const char* allIguanaExtensions[] = 
{
    "i",
    "ig",
    "iguana"
};

////////////////////////////////
// PRIVATE TYPES

TokenHandler_t* tokens;
TokenHandler_t* currentToken;
TokenHandler_t* endToken;
size_t tokensCount;
////////////////////////////////
// PRIVATE METHODS

static bool handleKeywordInteger_(ParserHandle_t parser, MainFrameHandle_t rootHandle, const Accessibility_t notation);
static inline bool handleNotation_(ParserHandle_t parser, MainFrameHandle_t rootHandle);
static inline bool postParsingJobsAST_(MainFrameHandle_t mainframe);

////////////////////////////////
// IMPLEMENTATION

/**
 * @brief   Public method for initializing Parser
 * 
 * @return  Success State
 */
bool Parser_initialize(ParserHandle_t parser)
{
    return SUCCESS;
}

bool Parser_destroy(ParserHandle_t parser)
{
    return SUCCESS;
}

/**
 * @brief Public method used for parsing tokens by provided token list
 * 
 * @param[out] root         Igauana code AST goes here as main root
 * @param[in] tokenList     file tokens go here 
 * @return                  Success state
 */ 
bool Parser_parseTokens(ParserHandle_t parser, MainFrameHandle_t root, const VectorHandler_t tokenVector)
{
    tokens = (TokenHandler_t*) tokenVector->expandable;
    tokensCount = tokenVector->currentSize;
    currentToken = tokens;

    if(!MainFrame_init(root))
    {
        Log_e(TAG, "MainFrame init failed");
        return ERROR;
    }

    while (cTokenType != END_FILE)
    {
        switch(cTokenType)
        {
            case BIT_TYPE: // detected bit keyword
            {
                if(!handleKeywordInteger_(parser, root, NO_NOTATION))
                {
                    return SUCCESS;
                }
            }break;

            case NOTATION: // detected annotation
            {
                if(!handleNotation_(parser, root))
                {
                    return SUCCESS;
                }
            }break;

            case SEMICOLON: break;                                          // detected random semicolon, skip it
            default : Shouter_shoutUnrecognizedToken(cTokenP);break;        // error case
        }

        currentToken++;

    }

    // Post parsing AST stuff 
    postParsingJobsAST_(root);

    return SUCCESS;
}

static inline bool handleNotation_(ParserHandle_t parser, MainFrameHandle_t rootHandle)
{
    currentToken++;

    if(NAMING)
    {
        // Identifying type by notation naming
        for(uint8_t bindingIdx = 0; bindingIdx < ObjectTypes_getNotationTableSize(); bindingIdx++)
        {
            if(strcmp((*currentToken)->valueString, ObjectTypes_getNotationBindingById(bindingIdx)->naming) == 0)
            {
                currentToken++;
                handleKeywordInteger_(parser, rootHandle, ObjectTypes_getNotationBindingById(bindingIdx)->type);
                return SUCCESS;
            }
        }

        // Situation when passed loop without finding anything
        Shouter_shoutError(cTokenP, "Notation '%s' is not existing in my knowledge", (*currentToken)->valueString);

        currentToken++;
        handleKeywordInteger_(parser, rootHandle, NO_NOTATION);
        
    }else
    {
        Shouter_shoutError(cTokenP, "Notation doesn't have any type declared");
    }
    return SUCCESS;
}


static inline bool handleKeywordInteger_(ParserHandle_t parser, MainFrameHandle_t rootHandle, const Accessibility_t notation)
{
    VariableObjectHandle_t variable;

    ALLOC_CHECK(variable, sizeof(VariableObject_t), ERROR);

    if(!VarParser_parseVariable(&currentToken, variable))
    {
        return ERROR;
    }

    
    currentToken++;

    if(cTokenType == SEMICOLON)
    {
        if(Hashmap_set(&rootHandle->classVariables, variable->objectName, variable))
        {
            Shouter_shoutError(cTokenP, "Variable \'%s\' is declared several times", variable->objectName);
            return ERROR;
        }
        
    }else if(cTokenType == EQUAL)
    {
        Shouter_shoutError(cTokenP, "Variables can be manipulated or assigned only in function scopes");

        if(!ParserUtils_skipUntil(&currentToken, SEMICOLON))
        {
            return ERROR;
        }
        
    }else if(cTokenType == BRACKET_ROUND_START)   // identified method
    {
        currentToken++;
        MethodParser_parseMethod(&currentToken, variable, parser, rootHandle, notation);
    }else
    {
        Shouter_shoutExpectedToken(cTokenP, SEMICOLON);
        if(!ParserUtils_skipUntil(&currentToken, SEMICOLON))
        {
            return ERROR;
        }
    }

    return SUCCESS;

}


static inline bool postParsingJobsAST_(MainFrameHandle_t mainframe)
{

    // Categorizing each bit pack variable to corresponding group
    // Assigning bitpack positions
    // Algorithm of packing should be decided depending on optimization
    if(!Bitfit_assignGroupsAndPositionForVariableHashmap_(&mainframe->classVariables, FIRST_FIT, &mainframe->objectSizeBits))
    {
        Log_e(TAG, "Failed to do bitfitting");
        return ERROR;
    }

    return SUCCESS;
}