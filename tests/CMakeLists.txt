# Test executable
add_executable(run_tests 
    test_lexer.cpp
    test_parser.cpp
    test_symbol_table.cpp
)
target_link_libraries(run_tests PRIVATE minicompiler_lib)
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# # Unit tests
# add_test(NAME unit_tests COMMAND run_tests)

# Ensure tests know where to find the test executable
set(TEST_EXECUTABLE $<TARGET_FILE:run_tests>)
add_test(NAME unit_tests COMMAND ${TEST_EXECUTABLE})