cmake_minimum_required(VERSION 3.10)
project(MiniCompiler VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/token.cpp
    src/symbol_table.cpp
    src/parser.cpp
    src/error.cpp
)

add_executable(minicompiler ${SOURCES})

install(TARGETS minicompiler DESTINATION bin)

enable_testing()
add_subdirectory(tests)