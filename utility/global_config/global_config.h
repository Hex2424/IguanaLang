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


/**
 * @brief Setting up development logging verbose level
 * 0 - (no verbose)
 * 1 - (ERROR)
 * 2 - (ERROR / WARN)
 * 3 - (INFO / DEBUG / ERROR / WARN)
 */
#define VERBOSE_LEVEL                   2
#define VERBOSE_C_COMPILER              1
#define ENABLE_TEMP_FILES_CLEANUP       0

// OS types     
#define WINDOWS                         1
#define LINUX                           2

#define OS                              LINUX

#define OBJECT_ID_LENGTH                8              // in bytes
#define MAX_RETRIES_ID                  15
#define OBJECT_RANDOM_SEED              0

#define LINUX_TEMP_FOLDER_PATH          "./"
#define WINDOWS_TEMP_FOLDER_PATH        "ECHO %Temp%" // get from CMD
#define MAIN_PROCESS_FILE_NAME          "___start"


#if OS == LINUX
    #define TEMP_PATH                   LINUX_TEMP_FOLDER_PATH
#elif OS == WINDOWS
    #define TEMP_PATH                   WINDOWS_TEMP_FOLDER_PATH
#endif

#define CFILES_LENGTH                   (sizeof(TEMP_PATH) + OBJECT_ID_LENGTH + sizeof(".x")) - 1

#endif // UTILITY_GLOBAL_CONFIG_GLOBAL_CONFIG_H_
