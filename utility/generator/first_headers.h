
/**
 * @file first_headers.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2025-04-25
 */

#ifndef UTILITY_FIRST_HEADERS_H_
#define UTILITY_FIRST_HEADERS_H_

#include <global_config.h>


// TODO: in future need implement args handling
static const char START_POINT[] = "extern void f(void* p) asm(\"" MAIN_PROCESS_FILE_NAME "\"); int entry_main(void){f((void*) 0);__builtin_exit(0);}\n";

#endif // UTILITY_FIRST_HEADERS_H_