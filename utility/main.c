/**
 * @file main.c
 *
 * MORE INFO ABOUT THE FILE'S CONTENTS
 *
 * @copyright This file is a part of the project Iguana and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-04
 */

#include "compiler/compiler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <argp.h>

const char *argp_program_version = "Iguana 1.0";
const char *argp_program_bug_address = "<markas.vielavicius@gmail.com>";
static char doc[] = "Iguana compiler options";
static char args_doc[] = "[FILES]...";
static struct argp_option options[] = { 
    { "output", 'o', 0, 0, "Destination executable path"},
    // { "word", 'w', 0, 0, "Compare words instead of characters."},
    // { "nocase", 'i', 0, 0, "Compare case insensitive instead of case sensitive."},
    { 0 } 
};

struct arguments {
    enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode;
    bool isCaseInsensitive;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'l': arguments->mode = LINE_MODE; break;
    case 'w': arguments->mode = WORD_MODE; break;
    case 'i': arguments->isCaseInsensitive = true; break;
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };


int main(int argc, char const *argv[])
{
    Compiler_t compiler;
    struct arguments arguments;
    arguments.mode = CHARACTER_MODE;
    arguments.isCaseInsensitive = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // if(!Compiler_initialize(&compiler))
    // {
    //     exit(-1);
    // }
    // compiler.mainIguanaFilePath = realpath(argv[0], NULL);

    // if(!Compiler_startCompilingProcessOnRoot(&compiler, filePathToCompile))
    // {
    //     Compiler_destroy(&compiler);
    //     exit(-2);
    // }

    // exit(Compiler_destroy(&compiler));
}