# Add test executable
set(TEST_SOURCES
    test_lexer.cpp
    test_parser.cpp
    test_symbol_table.cpp
    # Include source files directly instead of linking to a library
    ../src/lexer.cpp
    ../src/token.cpp
    ../src/symbol_table.cpp
    ../src/parser.cpp
    ../src/error.cpp
)

add_executable(run_tests ${TEST_SOURCES})

# Make sure it can find the header files
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add individual tests for each test file in test_files directory
file(GLOB TEST_FILES "test_files/*.c")
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_test(
        NAME ${TEST_NAME}
        COMMAND minicompiler ${TEST_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

# Add a test for the test executable
add_test(NAME unit_tests COMMAND run_tests)