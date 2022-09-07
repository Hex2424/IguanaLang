/**
 * @file mainframe.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-07
 */

#ifndef UTILITY_PARSER_STRUCTURES_MAIN_FRAME_MAINFRAME_H_
#define UTILITY_PARSER_STRUCTURES_MAIN_FRAME_MAINFRAME_H_
#include "../class/class.h"

typedef struct
{
    char* name;

}ImportObject_t;


typedef ImportObject_t* ImportObjectHandle_t;

typedef struct
{
    ImportObjectHandle_t* imports;
    ClassObjectHandle_t* classes;
    
}MainFrame_t;

typedef MainFrame_t* MainFrameHandle_t;



#endif // UTILITY_PARSER_STRUCTURES_MAIN_FRAME_MAINFRAME_H_