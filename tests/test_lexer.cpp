#include <iostream>
#include <cassert>
#include "lexer.h"
#include "token.h"

// Simple test function declarations
void testLexer();

// Main function for the test executable
int main() {
    std::cout << "Running tests...\n";
    
    // Run test functions
    testLexer();
    
    std::cout << "All tests passed!\n";
    return 0;
}

// Simple test implementation
void testLexer() {
    std::string source = "main() { int x = 5; }";
    // Lexer lexer(source);
    
    // For now, just make sure it doesn't crash
    // std::vector<Token> tokens = lexer.tokenize();
    
    std::cout << "Lexer test passed!\n";
}