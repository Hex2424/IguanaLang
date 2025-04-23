cmake_minimum_required(VERSION 3.10)


set(CMAKE_C_COMPILER /usr/bin/gcc)

set(PROJECT_NAME iguana)
project(${PROJECT_NAME} LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SRC 
    utility/main.c
    utility/separator/separator.c
    utility/tokenizer/tokenizer.c
    utility/logger/logger.c
    utility/vector/vector.c
    utility/compiler/compiler.c
    utility/parser/parser.c
    utility/file_reader/file_reader.c
    utility/parser/structures/main_frame/main_frame.c
    utility/parser/parser_utilities/compiler_messages.c
    utility/parser/parser_utilities/smaller_parsers/method_parser/method_parser.c
    utility/parser/parser_utilities/smaller_parsers/var_parser/var_parser.c
    utility/parser/parser_utilities/global_parser_utility.c
    utility/generator/generator.c
    utility/queue/queue.c
    utility/stack/stack.c
    utility/hash/random/random.c
    utility/external/inbuilt_c_compiler/c_compiler.c
    utility/external/unix_linker/unix_linker.c
    utility/hashmap/hashmap.c
    utility/parser/parser_utilities/smaller_parsers/body_parser/body_parser.c
    utility/parser/structures/object_type/object_type.c
    utility/symbol_table/symbol_table.c
)

include_directories(
    ${CMAKE_SOURCE_DIR}/utility/vector
    ${CMAKE_SOURCE_DIR}/utility/hashmap
    ${CMAKE_SOURCE_DIR}/utility/queue
    ${CMAKE_SOURCE_DIR}/utility/stack
    ${CMAKE_SOURCE_DIR}/utility/global_config
    ${CMAKE_SOURCE_DIR}/utility/logger
    ${CMAKE_SOURCE_DIR}/utility/misc)

if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
else()
    add_definitions(-DPLATFORM_LINUX)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

add_executable(${PROJECT_NAME} ${SRC})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
