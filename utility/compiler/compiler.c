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
#include "../parser/parser_utilities/compiler_messages.h"
#include <errno.h>

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
void Compiler_removeExtensionFromFilenameWithCopy_(char* filename, const char* const filenameWithExtension)
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

static int cfilenameOfObject_(char* cfilename, const char* objectName)
{
    return sprintf(cfilename, "%s.c", objectName);
}

/**
 * @brief Public method used to compile one Iguana file to C lang
 * 
 * @param[in/out] compiler - Compiler object itself
 * @param[in/out] filePath - Import object containing Iguana file path
 * @return bool            - Success state
 */
bool Compiler_compileIguana(const char* iguanaFilePath, const bool isFirstFile)
{
    char filenameGenerate[MAX_FILENAME_LENGTH];
    MainFrame_t root;

    Vector_t tokensVector;
    Parser_t parser;

    char* codeString;
    size_t length;

    // Setting a name for currently compile object
    Compiler_removeExtensionFromFilenameWithCopy_(root.iguanaObjectName, basename((char*) iguanaFilePath));
    cfilenameOfObject_(filenameGenerate, root.iguanaObjectName);
    
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

    if(Shouter_getErrorCount() == NO_ERROR)
    {
        // Generator generates code out of AST(Abstract syntax tree)
        if(!Generator_generateCode(&root, filenameGenerate, isFirstFile))
        {
            Log_e(TAG, "Failed to generate c language code for Iguana file %s", iguanaFilePath);
            return ERROR;
        }

    }else
    {
        Shouter_shoutError(NULL, "Compiling completed with %d errors", Shouter_getErrorCount());
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
