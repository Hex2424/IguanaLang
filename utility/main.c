/**
 * @file main.c
 *
 * Iguana Compiler - Entry Point
 *
 * @copyright This file is part of the project Iguana and is distributed under MIT license.
 * See: https://choosealicense.com/licenses/mit/
 *
 * @author Markas Vielavičius (markas.vielavicius@bytewall.com)
 *
 * @date 2022-09-04
 */

#include "compiler/compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>

const char *argp_program_version = "Iguana 1.0";
const char *argp_program_bug_address = "<markas.vielavicius@gmail.com>";
static char doc[] = "Iguana compiler options";
static char args_doc[] = "[FILES]...";

static struct argp_option options[] = {
    { "output", 'o', "FILE", 0, "Destination executable path" },
    { "only-c", 'c', 0, 0, "Only generate .c source files (no object files or executable)" },
    { "only-object", 'b', 0, 0, "Only compile to .o object files (no linking)" },
    { 0 }
};

// Define mode enumeration
enum Mode { CHARACTER_MODE, WORD_MODE, LINE_MODE };

struct arguments {
    enum Mode mode;
    bool isCaseInsensitive;
    bool only_c;
    bool only_obj;
    char *output_path;
    char **files;
    int file_count;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'o':
        arguments->output_path = arg;
        break;
    case 'c':
        arguments->only_c = true;
        break;
    case 'b':
        arguments->only_obj = true;
        break;
    case ARGP_KEY_ARG:
        arguments->files = realloc(arguments->files, (arguments->file_count + 1) * sizeof(char *));
        arguments->files[arguments->file_count++] = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char **argv) 
{
    struct arguments arguments = { CHARACTER_MODE, false, NULL, NULL, 0 };
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (arguments.file_count == 0) 
    {
        fprintf(stderr, "Error: No files specified for compilation.\n");
        return EXIT_FAILURE;
    }

    // REMOVE THIS VECTOR IN FUTURE ITS NOT NEEDED
    Vector_t pathsToLink;

    if(!Vector_create(&pathsToLink, NULL))
    {
        fprintf(stderr, "Error: failed to create vector for link paths\n");
        return EXIT_FAILURE;
    }

    // Process each file
    for (int argIdx = 0; argIdx < arguments.file_count; argIdx++) 
    {
        char* iguanaObjectName;


        iguanaObjectName = malloc(strlen(arguments.files[argIdx]));

        Compiler_removeExtensionFromFilenameWithCopy_(iguanaObjectName, basename((char*) arguments.files[argIdx]));
            
        // After compiled main file, secondary objects also need compile
        if(!Compiler_compileIguana(arguments.files[argIdx]))
        {
            fprintf(stderr, "Error to compile Iguana path: %s", arguments.files[argIdx]);
            return EXIT_FAILURE;
        }

        if(!arguments.only_c)
        {
            if(!CExternalCompiler_compile(iguanaObjectName, true))
            {
                fprintf(stderr, "Error to compile c file %s.c", iguanaObjectName);
                return EXIT_FAILURE;
            }

            if(!Vector_append(&pathsToLink, iguanaObjectName))
            {
                fprintf(stderr, "Error: failed to create vector for link paths\n");
                return EXIT_FAILURE;
            }
        }
    }

    if(!arguments.only_obj && !arguments.only_c)
    {  
        if(!UnixLinker_linkPaths(&pathsToLink))
        {
            fprintf(stderr, "Error failed to link objects...");
            return EXIT_FAILURE;
        }

    }
    
    free(arguments.files);

    return EXIT_SUCCESS;
}