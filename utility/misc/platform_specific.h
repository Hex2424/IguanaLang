#ifndef UTILITY_MISC_PLATFORM_SPECIFIC_H_
#define UTILITY_MISC_PLATFORM_SPECIFIC_H_

#define OBJECT_ID_LENGTH                8              // in bytes
#define MAX_RETRIES_ID                  15
#define OBJECT_RANDOM_SEED              0

#define LINUX_TEMP_FOLDER_PATH          "./"
#define WINDOWS_TEMP_FOLDER_PATH        "ECHO %Temp%" // get from CMD

#ifdef __linux__
    // LINUX specific includes
    #include <linux/limits.h>
    
    // LINUX specific defines
    #define PATH_MAX_LENGTH             PATH_MAX
    #define TEMP_PATH                   LINUX_TEMP_FOLDER_PATH
    #define MAX_FILENAME_LENGTH         NAME_MAX
    

#elif defined(_WIN32)
    // WINDOWS specific includes
    #include <windows.h>

    // WINDOWS specific defines
    #define PATH_MAX_LENGTH             MAX_PATH
    #define TEMP_PATH                   WINDOWS_TEMP_FOLDER_PATH
    #define MAX_FILENAME_LENGTH         _MAX_FNAME
#else 
    #include <sys/syslimits.h>
    #warning UNRECOGNISED OS detected
#endif


#endif