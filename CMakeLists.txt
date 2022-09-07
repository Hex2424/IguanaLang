
set(PROJECT_NAME Iguana)
set(PROJECT_LOCATION /home/hex24/Desktop/C_Projects/IguanaLang)
set(PROJECT_STRING "Iguana")
set(SRC 
utility/main.c
utility/separator/separator.c
utility/tokenizer/tokenizer.c
utility/logger/logger.c
utility/vector/vector.c
utility/compiler/compiler.c
utility/parser/parser.c
)

cmake_minimum_required(VERSION 3.3)
project(${PROJECT_NAME} LANGUAGES C)
add_executable(${PROJECT_STRING} ${SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)