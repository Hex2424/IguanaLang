/**
 * @file generator.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-11
 */

#ifndef UTILITY_GENERATOR_GENERATOR_H_
#define UTILITY_GENERATOR_GENERATOR_H_

#include "stdint.h"
#include "stdbool.h"
#include "../parser/structures/main_frame/main_frame.h"

bool Generator_generateCodeFor(MainFrameHandle_t ast);

#endif // UTILITY_GENERATOR_GENERATOR_H_