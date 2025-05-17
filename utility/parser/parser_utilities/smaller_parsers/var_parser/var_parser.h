/**
 * @file var_parser.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2025-04-19
 */

#ifndef UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_VAR_PARSER_H_
#define UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_VAR_PARSER_H_

#include "../../../structures/method/method.h"
#include "../../../structures/main_frame/main_frame.h"
#include "../../../../tokenizer/token/token.h"
#include "../../../../parser/parser.h"


bool VarParser_parseVariable(TokenHandler_t** currentToken, VariableObjectHandle_t variableHolder);
VariableObjectHandle_t VarParser_searchVariableInVectorByName(const VectorHandler_t vectorHandle, const char* name);
#endif // UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_VAR_PARSER_H_