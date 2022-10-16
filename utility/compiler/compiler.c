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
#include "../vector/vector.h"
#include "../file_reader/file_reader.h"
#include "../parser/parser.h"
#include "../generator/generator.h"
#include "compiler.h"
#include "string.h"

////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "COMPILER";

////////////////////////////////
// PRIVATE TYPES
static bool compileFile_(CompilerHandle_t compiler, const char* filePath);

////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION



static bool compileFile_(CompilerHandle_t compiler, const char* filePath)
{
    Vector_t tokensVector;
    CodeGenerator_t codeGenerator;
    Parser_t parser;
    char* codeString;
    size_t length;
    MainFrame_t root;

    parser.compiler = compiler;
    parser.currentFilePath = filePath;
    

    // TODO optimize this shit
    char* lastSlashPointer = strrchr(filePath, '/');
    if(lastSlashPointer == NULL)
    {
        parser.currentFolderPath = "";
        parser.currentFolderPathLength = 0;
    }else
    {
        parser.currentFolderPath = filePath;
        parser.currentFolderPathLength = lastSlashPointer - filePath + 1;
    }


    // TODO: read directly from file
    length = FileReader_readToBuffer(filePath, &codeString);
    if(length == -1)
    {
        Log_e(TAG, "Error occured in reading path:%s", filePath);
        return ERROR;
    }
    
    if(!Separator_getSeparatedWords(codeString, length, &tokensVector))
    {
        Log_e(TAG, "Seperator failed to parse: %s", codeString);
        return ERROR;
    }
    Vector_fit(&tokensVector);
    
    Vector_print(&tokensVector);

    // initializing parser object

    if(!Parser_initialize(&parser))
    {
        Log_e(TAG, "Failed to initialize parser");
        return ERROR;
    }

    if(!Generator_initialize(&codeGenerator, filePath, &root))
    {
        Log_e(TAG, "Failed to initialize code generator");
        return ERROR;
    }

    if(!Parser_parseTokens(&parser, &root, &tokensVector))
    {
        Log_e(TAG, "Failed to parse tokens");
        return ERROR;
    }


    
    if(!Generator_generateCode(&codeGenerator))
    {
        Log_e(TAG, "Failed to generate c language code for Iguana file %s", filePath);
        return ERROR;
    }

    if(!Vector_append(&compiler->alreadyCompiledFilePaths, filePath))
    {
        Log_e(TAG, "Failed to append an compiled file path");
        return ERROR;
    }

    if(!FileReader_destroy(codeString))
    {
        Log_e(TAG, "Failed to destroy codeString");
        return ERROR;
    }

    if(!Parser_destroy(&parser))
    {
        Log_w(TAG, "Failed to deallocate Parser object");
        return ERROR;
    }

    if(!Vector_destroy(&tokensVector))
    {
        Log_e(TAG, "Failed to destroy vector");
        return ERROR;
    }
    
    return SUCCESS;
}


bool Compiler_initialize(CompilerHandle_t compiler)
{
    if(!Vector_create(&compiler->alreadyCompiledFilePaths, NULL))
    {
        Log_e(TAG, "Failed to create vector for already compiler file paths");
        return ERROR;
    }

    if(!Queue_create(&compiler->filePathsToCompile))
    {
        Log_e(TAG, "Failed to create filePathsToCompilerQueue");
        return ERROR;
    }

    return SUCCESS;
}


bool Compiler_startCompilingProcessOnRoot(CompilerHandle_t compiler, const char* filePath)
{
    Queue_enqueue(&compiler->filePathsToCompile, filePath);

    while (true)
    {
        char* currentFilePath;

        currentFilePath = Queue_dequeue(&compiler->filePathsToCompile);

        if(currentFilePath == NULL)
        {
            break;  // compilation done
        }

        if(!compileFile_(compiler, currentFilePath))
        {
            Log_e(TAG, "Failed to compile %s", currentFilePath);
            return ERROR;
        }
    }
    return SUCCESS;

}

bool Compiler_destroy(CompilerHandle_t compiler)
{
    if(!Vector_destroy(&compiler->alreadyCompiledFilePaths))
    {
        Log_e(TAG, "Failed to destroy alreadyCompiledFilePaths vector");
        return ERROR;
    }

    Queue_destroy(&compiler->filePathsToCompile);

    return SUCCESS;
}