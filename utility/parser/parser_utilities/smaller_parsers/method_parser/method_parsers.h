/**
 * @file method_parsers.h
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

#ifndef UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_METHOD_PARSERS_H_
#define UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_METHOD_PARSERS_H_

#include "../../../structures/method/method.h"
#include "../../../structures/main_frame/main_frame.h"
#include "../../../../tokenizer/token/token.h"




bool MethodParser_parseMethod(TokenHandler_t** currentToken, MainFrameHandle_t root);

#endif // UTILITY_PARSER_PARSER_UTILITIES_SMALLER_PARSERS_METHOD_PARSERS_H_