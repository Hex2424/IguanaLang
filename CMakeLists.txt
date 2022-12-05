
set(PROJECT_NAME iguana)
set(PROJECT_LOCATION /home/hex24/Desktop/C_Projects/IguanaLang)
set(PROJECT_STRING "iguana")
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
utility/parser/parser_utilities/global_parser_utility.c
utility/generator/generator.c
utility/queue/queue.c
utility/hash/random/random.c
utility/hash/hash.c
utility/parser/structures/import_object/import_object.c
utility/external/inbuilt_c_compiler/c_compiler.c
utility/external/unix_linker/unix_linker.c
utility/hashmap/hashmap.c
utility/parser/parser_utilities/smaller_parsers/body_parser/body_parser.c
utility/file_manip/files.c
utility/parser/structures/object_type/object_type.c
)

cmake_minimum_required(VERSION 3.3)
project(${PROJECT_NAME} LANGUAGES C)
add_executable(${PROJECT_STRING} ${SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)