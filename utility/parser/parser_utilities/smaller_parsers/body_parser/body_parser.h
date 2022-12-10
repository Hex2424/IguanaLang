/**
 * @file body_parser.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-11-12
 */

#ifndef UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_METHOD_PARSER_BODY_PARSER_BODY_PARSER_H_
#define UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_METHOD_PARSER_BODY_PARSER_BODY_PARSER_H_
#include "../../../structures/method/method_scope/local_scope.h"
#include "../../../../tokenizer/token/token.h"
#include "../../../../hashmap/hashmap.h"
#include <stdbool.h>


bool BodyParser_parseScope(LocalScopeObjectHandle_t scopeBody, TokenHandler_t** currentTokenHandle);
bool BodyParser_initialize(LocalScopeObjectHandle_t scopeBody);

#endif // UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_METHOD_PARSER_BODY_PARSER_BODY_PARSER_H_