/**
 * @file global_config.h
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

#ifndef UTILITY_GLOBAL_CONFIG_GLOBAL_CONFIG_H_
#define UTILITY_GLOBAL_CONFIG_GLOBAL_CONFIG_H_
#include <platform_specific.h>

/**
 * @brief Setting up development logging verbose level
 * 0 - (no verbose)
 * 1 - (ERROR)
 * 2 - (ERROR / WARN)
 * 3 - (INFO / DEBUG / ERROR / WARN)
 */
#define VERBOSE_LEVEL                   3
#define VERBOSE_C_COMPILER              1
#define ENABLE_TEMP_FILES_CLEANUP       1


#define OBJECT_ID_LENGTH                8              // in bytes
#define MAX_RETRIES_ID                  15
#define OBJECT_RANDOM_SEED              0

#define MAIN_PROCESS_FILE_NAME          "_ZN9bit0_main4mainEv"

#define PARAMS_VAR_REGION_NAME          "params"
#define CLASS_VAR_REGION_NAME           "object"
#define LOCAL_VAR_REGION_NAME           "local"

#define CFILES_LENGTH                   MAX_FILENAME_LENGTH

#endif // UTILITY_GLOBAL_CONFIG_GLOBAL_CONFIG_H_
