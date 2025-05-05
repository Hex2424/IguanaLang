/**
 * @file colors.h
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Hex2424
 *
 * @date 2022-09-05
 */

#ifndef UTILITY_LOGGER_COLORS_H_
#define UTILITY_LOGGER_COLORS_H_

typedef const char* const color_t;


static color_t BLACK =        "\033[0;30m";
static color_t RED =          "\033[0;31m";
static color_t GREEN =        "\033[0;32m";
static color_t BROWN =        "\033[0;33m";
static color_t BLUE =         "\033[0;34m";
static color_t PURPLE =       "\033[0;35m";
static color_t CYAN =         "\033[0;36m";
static color_t LIGHT_GRAY =   "\033[0;37m";
static color_t DARK_GRAY =    "\033[1;30m";
static color_t LIGHT_RED =    "\033[1;31m";
static color_t LIGHT_GREEN =  "\033[1;32m";
static color_t YELLOW =       "\033[1;33m";
static color_t LIGHT_BLUE =   "\033[1;34m";
static color_t LIGHT_PURPLE = "\033[1;35m";
static color_t LIGHT_CYAN =   "\033[1;36m";
static color_t LIGHT_WHITE =  "\033[1;37m";
static color_t BOLD =         "\033[1m";
static color_t FAINT =        "\033[2m";
static color_t ITALIC =       "\033[3m";
static color_t UNDERLINE =    "\033[4m";
static color_t BLINK =        "\033[5m";
static color_t CROSSED =      "\033[9m";
static color_t END =          "\033[0m";



#endif // UTILITY_LOGGER_COLORS_H_