/**
 * @file compiler.c
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
#include "../separator/separator.h"
#include <vector.h>
#include "../file_reader/file_reader.h"
#include "../parser/parser.h"
#include "../generator/generator.h"
#include "compiler.h"
#include "string.h"
#include <global_config.h>
#include "../external/inbuilt_c_compiler/c_compiler.h"
#include "../external/unix_linker/unix_linker.h"
#include "../parser/parser_utilities/compiler_messages.h"
#include <errno.h>
#include <libgen.h>
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "COMPILER";

////////////////////////////////
// PRIVATE TYPES

// static bool cleanTempFilePaths_();
// static bool cleanTempCFile_(ImportObjectHandle_t currentImport);
////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION
static void removeExtensionFromFilenameWithCopy_(char* filename, const char* const filenameWithExtension)
{
    
    strcpy(filename, filenameWithExtension);
    
    char* dotPointer = strchr(filename, '.');

    if(dotPointer == NULL)
    {
        // not found a dot pointer
        return;
    }

    // placing NULL terminator so string will count to terminator
    *dotPointer = '\0';
}

/**
 * @brief Public method used to compile one Iguana file to C lang
 * 
 * @param[in/out] compiler - Compiler object itself
 * @param[in/out] filePath - Import object containing Iguana file path
 * @return bool            - Success state
 */
bool Compiler_compile(const char* iguanaFilePath, const bool isMainFile)
{

    MainFrame_t root;

    Vector_t tokensVector;
    Parser_t parser;

    char* codeString;
    size_t length;

    // Setting a name for currently compile object
    removeExtensionFromFilenameWithCopy_(root.iguanaObjectName, basename((char*) iguanaFilePath));

    // TODO: tokenize directly from file
    length = FileReader_readToBuffer(iguanaFilePath, &codeString);
    if(length == -1)
    {
        Log_e(TAG, "Error occured in reading path:%s", iguanaFilePath);
        return ERROR;
    }
    
    // Seperator works as tokenizer - converts file to tokens
    if(!Separator_getSeparatedWords(codeString, length, &tokensVector, iguanaFilePath))
    {
        Log_e(TAG, "Separator failed to parse: %s", codeString);
        return ERROR;
    }

    Vector_fit(&tokensVector);
    
    Vector_print(&tokensVector);
    
    // Initializing parser object
    if(!Parser_initialize(&parser))
    {
        Log_e(TAG, "Failed to initialize parser");
        return ERROR;
    }

    // Parser parse tokens to Abstract Syntax Tree
    if(!Parser_parseTokens(&parser, &root, &tokensVector))
    {
        Log_e(TAG, "Failed to parse tokens");
        return ERROR;
    }

    // Generator generates code out of AST(Abstract syntax tree)
    if(!Generator_generateCode(&root, "main.c"))
    {
        Log_e(TAG, "Failed to generate c language code for Iguana file %s", iguanaFilePath);
        return ERROR;
    }

    // cleanTempCFile_(filePath);

    // Deallocating file buffer
    if(!FileReader_destroy(codeString))
    {
        Log_e(TAG, "Failed to destroy codeString");
        return ERROR;
    }

    // Deallocating Parser resources
    if(!Parser_destroy(&parser))
    {
        Log_w(TAG, "Failed to deallocate Parser object");
        return ERROR;
    }

    // Deallocating tokens vector
    if(!Vector_destroy(&tokensVector))
    {
        Log_e(TAG, "Failed to destroy vector");
        return ERROR;
    }

    
    return SUCCESS;
}


// /**
//  * @brief Private method for creating start .c file which will run main object (Wrapper)
//  * 
//  * @param[in/out] compiler      - Compiler object for initializing
//  * @param[in] mainFileName      - Main process file name
//  * @return bool                 - Success state
//  */
// bool Compiler_initialize(const char* mainFilePath)
// {
//     FILE* file;
//     char fileName[CFILES_LENGTH];
//     char* iguanaFileName;

//     // getting real name of path
//     iguanaFileName = basename((char*) mainFilePath);
    
//     char *dot = strrchr(iguanaFileName, '.');
//     if (dot && dot != iguanaFileName) {
//         *dot = '\0'; // Remove file extension
//     }

//     fileName[0] = '\0';
//     sprintf(fileName, "%s%s.c", TEMP_PATH, MAIN_PROCESS_FILE_NAME);
  
//     file = fopen(fileName, "w");

//     if(file == NULL)
//     {
//         Log_e(TAG, "Failed to create file");
//         return ERROR;
//     }

//     // adding mainProccess file for executing main object
//     fprintf(file, "void exit(int);int %s(){exit(_ZN%u%s%u%sEv((void*)0));}",
//         MAIN_PROCESS_FILE_NAME,
//         strlen(iguanaFileName),
//         iguanaFileName,
//         strlen(iguanaFileName),
//         iguanaFileName
//     );

//     if(fclose(file) != 0)
//     {
//         Log_w(TAG, "Failed to close file: %s", fileName);
//     }

//     return SUCCESS;
// }

// /**
//  * @brief Public method for starting main compiling process
//  *
//  * @param[in] filePath      - File path of main Iguana file which has "bit:32 main()" method
//  * @return bool             - Success state
//  */
// bool Compiler_processAll(const char** filePaths)
// {
//     ImportObjectHandle_t mainImport;

//     ALLOC_CHECK(mainImport, sizeof(ImportObject_t), ERROR);
    
//     mainImport->name = (char*) filePath;
    
//     mainImport->realPath = realpath(filePath, NULL);

//     if(mainImport->realPath == NULL)
//     {
//         Log_e(TAG, "Failed to retrieve real path of %s %s", filePath, strerror(errno));
//         return ERROR;
//     }

//     if(!compileFile_(compiler, object))
//     {
//         Log_e(TAG, "Failed to compile %s with %s", object->name, object->objectId.id);
//         return ERROR;
//     }
        

//     if(Shouter_getErrorCount() == NO_ERROR)
//     {
//         if(!createMainProcessFile_(compiler, "main"))
//         {
//             Log_e(TAG, "Failed to initialize main process files");
//             return  ERROR;
//         }

//         // running external tools
//         if(!CExternalCompiler_compileWhole(&compiler->alreadyCompiledFilePaths))
//         {
//             Log_e(TAG, "C code compilation failed");
//             // cleanTempCFile_(filePath);
//             return ERROR;
//         }

//         if(!UnixLinker_linkPaths(&compiler->alreadyCompiledFilePaths))
//         {
//             Log_e(TAG, "Object files linking failed");
//             return ERROR;
//         }

//     }else
//     {
//         Shouter_shoutError(NULL, "Compiling completed with %d errors", Shouter_getErrorCount());
//     }
    


//     return SUCCESS;

// }

// static inline bool cleanTempFilePaths_()
// {
//     ImportObjectHandle_t compiledPathHandle;
//     char path[CFILES_LENGTH];
//     memcpy(path, TEMP_PATH, sizeof(TEMP_PATH) - 1);
//     path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH) - 1] = '.';
//     path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH) + 1] = '\0';

//     for(size_t compiledPathIdx = 0; compiledPathIdx < compiler->alreadyCompiledFilePaths.currentSize; compiledPathIdx++)
//     {
//         // uint8_t objectLength;

//         compiledPathHandle = compiler->alreadyCompiledFilePaths.expandable[compiledPathIdx];
//         // objectLength = strlen(compiledPathHandle->objectId.id);

//         memcpy(path + sizeof(TEMP_PATH) - 1, compiledPathHandle->objectId.id, OBJECT_ID_LENGTH);
//         path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH)] = 'c';

//         if(remove(path))
//         {
//             Log_d(TAG, "Failed to remove file: %s", path);
//         }

//         path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH)] = 'o';

//         if(remove(path))
//         {
//             Log_d(TAG, "Failed to remove file: %s", path);
//         }

//     }
//     return SUCCESS;
// }