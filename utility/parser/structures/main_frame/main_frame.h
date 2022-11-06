/**
 * @file mainframe.h
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

#ifndef UTILITY_PARSER_STRUCTURES_MAIN_FRAME_MAINFRAME_H_
#define UTILITY_PARSER_STRUCTURES_MAIN_FRAME_MAINFRAME_H_
#include "../variable/variable.h"
#include "../method/method.h"
#include "../../../hashmap/hashmap.h"



typedef struct
{
    Hashmap_t imports;
    Hashmap_t classVariables;
    Hashmap_t methods;
}MainFrame_t;

typedef MainFrame_t* MainFrameHandle_t;


bool MainFrame_destroy(MainFrameHandle_t handle);
bool MainFrame_init(MainFrameHandle_t handle);

#endif // UTILITY_PARSER_STRUCTURES_MAIN_FRAME_MAINFRAME_H_