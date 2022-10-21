/**
 * @file unix_linker.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-10-21
 */

#ifndef UTILITY_EXTERNAL_UNIX_LINKER_UNIX_LINKER_H_
#define UTILITY_EXTERNAL_UNIX_LINKER_UNIX_LINKER_H_
#include "../../vector/vector.h"

bool UnixLinker_linkPaths(const VectorHandler_t objectsCompiledExternaly);

#endif // UTILITY_EXTERNAL_UNIX_LINKER_UNIX_LINKER_H_