# Test executable
add_executable(run_tests 
    test_lexer.cpp
    test_parser.cpp
    test_symbol_table.cpp
)
target_link_libraries(run_tests PRIVATE minicompiler_lib)
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Ensure tests know where to find the test executable
set(TEST_EXECUTABLE $<TARGET_FILE:run_tests>)

# Add specific test cases
add_test(NAME lexer_tests COMMAND ${TEST_EXECUTABLE} "lexer")
add_test(NAME parser_tests COMMAND ${TEST_EXECUTABLE} "parser")
add_test(NAME symbol_table_tests COMMAND ${TEST_EXECUTABLE} "symbol_table")

# Also add a test that runs all test suites
add_test(NAME unit_tests COMMAND ${TEST_EXECUTABLE})

# Set working directory for tests to the build dir root
# This ensures the test_files directory is found correctly
set_tests_properties(
    lexer_tests 
    parser_tests 
    symbol_table_tests 
    unit_tests
    PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Copy test files to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_files DESTINATION ${CMAKE_BINARY_DIR})

# Configure test timeouts (5 minutes)
set_tests_properties(
    lexer_tests 
    parser_tests 
    symbol_table_tests 
    unit_tests
    PROPERTIES TIMEOUT 300
)

# Add a custom target to run tests with verbose output
add_custom_target(check 
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose 
    DEPENDS run_tests
)

message(STATUS "Tests configured. Run with 'ctest' or 'make check' after building.")
message(STATUS "Test files copied to: ${CMAKE_BINARY_DIR}/test_files/")