cmake_minimum_required(VERSION 3.10)
project(MiniCompiler VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a library for shared sources
add_library(minicompiler_lib STATIC
    src/lexer.cpp
    src/token.cpp
    src/symbol_table.cpp
    src/parser.cpp
    src/error.cpp
)

# Main executable
add_executable(minicompiler src/main.cpp)
target_link_libraries(minicompiler PRIVATE minicompiler_lib)
target_include_directories(minicompiler_lib PUBLIC include)

install(TARGETS minicompiler DESTINATION bin)

enable_testing()
add_subdirectory(tests)