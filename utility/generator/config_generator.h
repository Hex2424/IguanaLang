/**
 * @file config_generator.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2025-04-21
 */

#ifndef UTILITY_MISC_CONFIG_GENERATOR_H_
#define UTILITY_MISC_CONFIG_GENERATOR_H_

#define ENABLE_READABILITY          1

#define BITPACK_TYPE_EXP            Bitpack_t
#define BITPACK_TYPE_NAME           STRINGIFY(BITPACK_TYPE_EXP)
#define FUNCTION_OBJ_NAME           CLASS_VAR_REGION_NAME
#define FUNCTION_PARAM_NAME         PARAMS_VAR_REGION_NAME

#define TMP_VAR                     tmp
#define TMP_FUNC                    f
#endif // UTILITY_MISC_CONFIG_GENERATOR_H_