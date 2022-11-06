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
#include "../parser/structures/import_object/import_object.h"
#include "../global_config/global_config.h"
#include "../external/inbuilt_c_compiler/c_compiler.h"
#include "../external/unix_linker/unix_linker.h"
#include "../parser/parser_utilities/compiler_messages.h"
#include <errno.h>
////////////////////////////////
// DEFINES


////////////////////////////////
// PRIVATE CONSTANTS
static const char* TAG = "COMPILER";

////////////////////////////////
// PRIVATE TYPES

static bool compileFile_(CompilerHandle_t compiler, ImportObjectHandle_t filePath);
static bool checkIfPathAlreadyCompiled_(CompilerHandle_t compiler, ImportObjectHandle_t path);
static bool cleanTempFilePaths_(CompilerHandle_t compiler);
static bool createMainProcessFile_(CompilerHandle_t compiler,const char* mainFileName);
// static bool cleanTempCFile_(ImportObjectHandle_t currentImport);
////////////////////////////////
// PRIVATE METHODS


////////////////////////////////
// IMPLEMENTATION



static bool compileFile_(CompilerHandle_t compiler, ImportObjectHandle_t filePath)
{
    Vector_t tokensVector;
    CodeGenerator_t codeGenerator;
    Parser_t parser;
    char* codeString;
    size_t length;
    MainFrame_t root;

    parser.compiler = compiler;
    parser.currentFilePath = filePath->realPath;
    

    // TODO optimize this shit
    char* lastSlashPointer = strrchr(filePath->realPath, '/');
    if(lastSlashPointer == NULL)
    {
        parser.currentFolderPath = "";
        parser.currentFolderPathLength = 0;
    }else
    {
        parser.currentFolderPath = filePath->realPath;
        parser.currentFolderPathLength = lastSlashPointer - filePath->realPath + 1;
    }


    // TODO: read directly from file
    length = FileReader_readToBuffer(filePath->realPath, &codeString);
    if(length == -1)
    {
        Log_e(TAG, "Error occured in reading path:%s", filePath);
        return ERROR;
    }
    
    if(!Separator_getSeparatedWords(codeString, length, &tokensVector, filePath->realPath))
    {
        Log_e(TAG, "Seperator failed to parse: %s", codeString);
        return ERROR;
    }

    Vector_fit(&tokensVector);
    
    Vector_print(&tokensVector);

    if(!Vector_append(&compiler->alreadyCompiledFilePaths, filePath))
    {
        Log_e(TAG, "Failed to append an compiled file path");
        return ERROR;
    }

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
        Log_e(TAG, "Failed to generate c language code for Iguana file %s", filePath->realPath);
        return ERROR;
    }

    // cleanTempCFile_(filePath);

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
    Random_fast_srand();

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

static inline bool createMainProcessFile_(CompilerHandle_t compiler, const char* mainFileName)
{
    FILE* file;
    char fileName[CFILES_LENGTH];

    fileName[0] = '\0';
    strcat(fileName, TEMP_PATH);
    strcat(fileName, MAIN_PROCESS_FILE_NAME);
    strcat(fileName, ".c");

    file = fopen(fileName, "w");

    if(file == NULL)
    {
        Log_e(TAG, "Failed to create file");
        return ERROR;
    }

    // adding mainProccess file for executing main object
    fprintf(file,
    "#include \"%s.h\"\nvoid exit(int);int %s(){exit(%s_%s());}",
    ((ImportObjectHandle_t)compiler->alreadyCompiledFilePaths.expandable[0])->objectId.id,
    MAIN_PROCESS_FILE_NAME,
    ((ImportObjectHandle_t)compiler->alreadyCompiledFilePaths.expandable[0])->objectId.id,
    mainFileName);

    if(fclose(file) != 0)
    {
        Log_w(TAG, "Failed to close file: %s", fileName);
    }

    fileName[strlen(fileName) - 1] = 'h';
    file = fopen(fileName, "w");
    
    if(fclose(file) != 0)
    {
        Log_w(TAG, "Failed to close file: %s", fileName);
    }
    ImportObjectHandle_t mainProcessImport;
    mainProcessImport = malloc(sizeof(ImportObject_t));
    ALLOC_CHECK(mainProcessImport, ERROR);

    mainProcessImport->name = NULL;
    strcpy(mainProcessImport->objectId.id, MAIN_PROCESS_FILE_NAME);
    mainProcessImport->realPath = NULL;

    if(!Vector_append(&compiler->alreadyCompiledFilePaths, mainProcessImport))
    {
        Log_e(TAG, "Failed to append main process import to vector");
        return ERROR;
    }
    return SUCCESS;
}


bool Compiler_startCompilingProcessOnRoot(CompilerHandle_t compiler, const char* filePath)
{
    ImportObjectHandle_t mainImport;
    mainImport = malloc(sizeof(ImportObject_t));
    ALLOC_CHECK(mainImport, ERROR);
    
    mainImport->name = filePath;
    
    mainImport->realPath = realpath(filePath, NULL);

    if(mainImport->realPath == NULL)
    {
        Log_e(TAG, "Failed to retrieve real path of %s %s", filePath, strerror(errno));
        return ERROR;
    }

    ImportObject_generateRandomIDForObject(mainImport);
    Queue_enqueue(&compiler->filePathsToCompile, mainImport);

    // Iguana compiling process
    while (true)
    {
        ImportObjectHandle_t object;

        object = Queue_dequeue(&compiler->filePathsToCompile);

        if(object == NULL)
        {
            break;  // compilation done
        }

        if(!compileFile_(compiler, object))
        {
            Log_e(TAG, "Failed to compile %s with %s", object->name, object->objectId.id);
            return ERROR;
        }
        
    }

    if(Shouter_getErrorCount() == NO_ERROR)
    {
        if(!createMainProcessFile_(compiler, "main"))
        {
            Log_e(TAG, "Failed to initialize main process files");
            return  ERROR;
        }

        // running external tools
        if(!CExternalCompiler_compileWhole(&compiler->alreadyCompiledFilePaths))
        {
            Log_e(TAG, "C code compilation failed");
            // cleanTempCFile_(filePath);
            return ERROR;
        }

        if(!UnixLinker_linkPaths(&compiler->alreadyCompiledFilePaths))
        {
            Log_e(TAG, "Object files linking failed");
            return ERROR;
        }

    }else
    {
        Shouter_shoutError(NULL, "Compiling completed with %d errors", Shouter_getErrorCount());
    }
    


    return SUCCESS;

}

bool Compiler_destroy(CompilerHandle_t compiler)
{
    cleanTempFilePaths_(compiler);

    if(!Vector_destroy(&compiler->alreadyCompiledFilePaths))
    {
        Log_e(TAG, "Failed to destroy alreadyCompiledFilePaths vector");
        return ERROR;
    }

    Queue_destroy(&compiler->filePathsToCompile);

    return SUCCESS;
}

static inline bool cleanTempFilePaths_(CompilerHandle_t compiler)
{
    ImportObjectHandle_t compiledPathHandle;
    char path[CFILES_LENGTH];
    memcpy(path, TEMP_PATH, sizeof(TEMP_PATH) - 1);
    path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH) - 1] = '.';
    path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH) + 1] = '\0';

    for(size_t compiledPathIdx = 0; compiledPathIdx < compiler->alreadyCompiledFilePaths.currentSize; compiledPathIdx++)
    {
        // uint8_t objectLength;

        compiledPathHandle = compiler->alreadyCompiledFilePaths.expandable[compiledPathIdx];
        // objectLength = strlen(compiledPathHandle->objectId.id);

        memcpy(path + sizeof(TEMP_PATH) - 1, compiledPathHandle->objectId.id, OBJECT_ID_LENGTH);
        path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH)] = 'c';

        if(remove(path))
        {
            Log_d(TAG, "Failed to remove file: %s", path);
        }

        path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH)] = 'h';

        if(remove(path))
        {
            Log_d(TAG, "Failed to remove file: %s", path);
        }

        path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH)] = 'o';

        if(remove(path))
        {
            Log_d(TAG, "Failed to remove file: %s", path);
        }

    }
    return SUCCESS;
}

// static inline bool cleanTempCFile_(ImportObjectHandle_t currentImport)
// {
//     char path[CFILES_LENGTH];
//     memcpy(path, TEMP_PATH, sizeof(TEMP_PATH) - 1);
//     path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH) - 1] = '.';
//     path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH) + 1] = '\0';

//     memcpy(path + sizeof(TEMP_PATH) - 1, currentImport->objectId.id, OBJECT_ID_LENGTH);
//     path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH)] = 'c';

//     if(remove(path))
//     {
//         Log_w(TAG, "Failed to close file: %s", path);
//     }

//     path[OBJECT_ID_LENGTH + sizeof(TEMP_PATH)] = 'h';

//     if(remove(path))
//     {
//         Log_w(TAG, "Failed to close file: %s", path);
//     }

//     return SUCCESS;
// }