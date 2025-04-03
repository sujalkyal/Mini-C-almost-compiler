#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"

// Implementation status flags
const bool LEXER_IMPLEMENTED = true;
const bool PARSER_IMPLEMENTED = false;  // Set to true when parser is implemented
const bool SYMBOL_TABLE_IMPLEMENTED = false;  // Set to true when symbol table is implemented

// Forward declarations for test functions
void testLexer();
void testParser();
void testSymbolTable();

int main(int argc, char* argv[]) {
    // If a specific test suite is requested, run only that
    if (argc > 1) {
        std::string testSuite = argv[1];
        
        if (testSuite == "lexer") {
            if (LEXER_IMPLEMENTED) {
                std::cout << "Running lexer tests...\n";
                testLexer();
            } else {
                std::cout << "Lexer not implemented yet.\n";
            }
            return 0;
        }
        
        if (testSuite == "parser") {
            if (PARSER_IMPLEMENTED) {
                std::cout << "Running parser tests...\n";
                testParser();
            } else {
                std::cout << "Parser not implemented yet.\n";
            }
            return 0;
        }
        
        if (testSuite == "symbol_table") {
            if (SYMBOL_TABLE_IMPLEMENTED) {
                std::cout << "Running symbol table tests...\n";
                testSymbolTable();
            } else {
                std::cout << "Symbol table not implemented yet.\n";
            }
            return 0;
        }
        
        std::cout << "Unknown test suite: " << testSuite << std::endl;
        std::cout << "Available test suites: lexer, parser, symbol_table" << std::endl;
        return 1;
    }
    
    // If no specific suite is requested, run all implemented tests
    std::cout << "Running all implemented test suites...\n\n";
    
    if (LEXER_IMPLEMENTED) {
        std::cout << "=== LEXER TESTS ===\n";
        testLexer();
        std::cout << "\n";
    } else {
        std::cout << "=== LEXER TESTS: Not implemented ===\n\n";
    }
    
    if (PARSER_IMPLEMENTED) {
        std::cout << "=== PARSER TESTS ===\n";
        testParser();
        std::cout << "\n";
    } else {
        std::cout << "=== PARSER TESTS: Not implemented ===\n\n";
    }
    
    if (SYMBOL_TABLE_IMPLEMENTED) {
        std::cout << "=== SYMBOL TABLE TESTS ===\n";
        testSymbolTable();
        std::cout << "\n";
    } else {
        std::cout << "=== SYMBOL TABLE TESTS: Not implemented ===\n\n";
    }
    
    std::cout << "All tests completed.\n";
    
    return 0;
} 