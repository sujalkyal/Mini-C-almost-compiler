#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "parser.h"
#include "lexer.h"
#include "error.h"

// This file will contain parser tests
// No main() function is needed here since test_lexer.cpp already has one

// Helper function to create a temporary file with the given source code
std::string createParserTestFile(const std::string& source) {
    std::string tempFilename = "temp_parser_test.c";
    std::ofstream file(tempFilename);
    file << source;
    file.close();
    return tempFilename;
}

// Test the FIRST sets calculation
void testFirstSets() {
    std::cout << "Testing FIRST sets calculation..." << std::endl;
    
    // Create a parser object
    std::string source = "int main() { int x = 0; }";
    std::string filename = createParserTestFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    // Get the first/follow sets
    const FirstFollowSets& sets = parser.getFirstFollowSets();
    
    // Test that FIRST(PROGRAM) contains the 'int' keyword
    const auto& firstProgram = sets.getFirstSet(NonTerminal::PROGRAM);
    assert(firstProgram.find(TokenType::Keyword) != firstProgram.end());
    
    // Test that FIRST(STATEMENT) contains various statement starters
    const auto& firstStatement = sets.getFirstSet(NonTerminal::STATEMENT);
    assert(firstStatement.find(TokenType::Keyword) != firstStatement.end()); // int, float
    assert(firstStatement.find(TokenType::Identifier) != firstStatement.end()); // assignments
    
    // Test that FIRST(EXPRESSION) contains appropriate tokens
    const auto& firstExpr = sets.getFirstSet(NonTerminal::EXPRESSION);
    assert(firstExpr.find(TokenType::Identifier) != firstExpr.end());
    assert(firstExpr.find(TokenType::IntegerLiteral) != firstExpr.end());
    assert(firstExpr.find(TokenType::FloatLiteral) != firstExpr.end());
    assert(firstExpr.find(TokenType::Punctuation) != firstExpr.end()); // For parentheses
    
    std::cout << "FIRST sets test passed!" << std::endl;
}

// Test the FOLLOW sets calculation
void testFollowSets() {
    std::cout << "Testing FOLLOW sets calculation..." << std::endl;
    
    // Create a parser object
    std::string source = "int main() { int x = 0; while (x < 10) { x = x + 1; } }";
    std::string filename = createParserTestFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    // Get the first/follow sets
    const FirstFollowSets& sets = parser.getFirstFollowSets();
    
    // Test that FOLLOW(STATEMENT) contains closing braces and semicolons
    const auto& followStatement = sets.getFollowSet(NonTerminal::STATEMENT);
    assert(followStatement.find(TokenType::Punctuation) != followStatement.end()); // For closing braces
    assert(followStatement.find(TokenType::Operator) != followStatement.end()); // For semicolons
    
    // Test that FOLLOW(EXPRESSION) contains appropriate tokens
    const auto& followExpr = sets.getFollowSet(NonTerminal::EXPRESSION);
    assert(followExpr.find(TokenType::Punctuation) != followExpr.end()); // For closing parentheses
    assert(followExpr.find(TokenType::Operator) != followExpr.end()); // For semicolons and operators
    
    std::cout << "FOLLOW sets test passed!" << std::endl;
}

// Test parsing of a simple program
void testParseSimpleProgram() {
    std::cout << "Testing parsing of a simple program..." << std::endl;
    
    std::string source = 
        "int main() {\n"
        "    int x = 5;\n"
        "    return 0;\n"
        "}\n";
    
    std::string filename = createParserTestFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    bool success = parser.parse();
    assert(success);
    assert(errorReporter.getErrorCount() == 0);
    
    std::cout << "Simple program parsing test passed!" << std::endl;
}

// Test parsing of a program with a while loop
void testParseWhileLoop() {
    std::cout << "Testing parsing of a program with a while loop..." << std::endl;
    
    std::string source = 
        "int main() {\n"
        "    int i = 0;\n"
        "    int sum = 0;\n"
        "    \n"
        "    while (i < 10) {\n"
        "        sum = sum + i;\n"
        "        i++;\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}\n";
    
    std::string filename = createParserTestFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    bool success = parser.parse();
    assert(success);
    assert(errorReporter.getErrorCount() == 0);
    
    std::cout << "While loop parsing test passed!" << std::endl;
}

// Test parsing of all arithmetic operators
void testParseArithmeticOperators() {
    std::cout << "Testing parsing of arithmetic operators..." << std::endl;
    
    std::string source = 
        "int main() {\n"
        "    int a = 5;\n"
        "    int b = 10;\n"
        "    int c = 0;\n"
        "    \n"
        "    c = a + b;\n"
        "    c = a - b;\n"
        "    c = a * b;\n"
        "    c = a / b;\n"
        "    a++;\n"
        "    b--;\n"
        "    \n"
        "    return 0;\n"
        "}\n";
    
    std::string filename = createParserTestFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    bool success = parser.parse();
    assert(success);
    assert(errorReporter.getErrorCount() == 0);
    
    std::cout << "Arithmetic operators parsing test passed!" << std::endl;
}

// Test parsing of programs with errors
void testParseErrors() {
    std::cout << "Testing parsing of programs with errors..." << std::endl;
    
    // Missing semicolon
    std::string source1 = 
        "int main() {\n"
        "    int x = 5\n"  // Missing semicolon
        "    return 0;\n"
        "}\n";
    
    std::string filename1 = createParserTestFile(source1);
    
    errorReporter.init(filename1);
    Lexer lexer1(filename1);
    TokenStream tokens1 = lexer1.tokenize();
    Parser parser1(tokens1, errorReporter);
    
    bool success1 = parser1.parse();
    assert(!success1 || errorReporter.getErrorCount() > 0);
    
    // Reset error reporter
    errorReporter.init(filename1);
    
    // Missing closing brace
    std::string source2 = 
        "int main() {\n"
        "    int x = 5;\n"
        "    while (x > 0) {\n"
        "        x--;\n"
        "    \n"  // Missing closing brace
        "    return 0;\n"
        "}\n";
    
    std::string filename2 = createParserTestFile(source2);
    
    errorReporter.init(filename2);
    Lexer lexer2(filename2);
    TokenStream tokens2 = lexer2.tokenize();
    Parser parser2(tokens2, errorReporter);
    
    bool success2 = parser2.parse();
    assert(!success2 || errorReporter.getErrorCount() > 0);
    
    std::cout << "Error handling test passed!" << std::endl;
}

// Test the panic mode error recovery
void testErrorRecovery() {
    std::cout << "Testing panic mode error recovery..." << std::endl;
    
    // Program with multiple errors
    std::string source = 
        "int main() {\n"
        "    int x = 5\n"  // Missing semicolon
        "    while (x > 0) {\n"
        "        x--\n"    // Missing semicolon
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}\n";
    
    std::string filename = createParserTestFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    bool success = parser.parse();
    
    // We should have errors but still reach the end of parsing
    assert(!success);
    assert(errorReporter.getErrorCount() > 0);
    
    std::cout << "Error recovery test passed!" << std::endl;
}

// Test relational operators in conditions
void testParseRelationalOperators() {
    std::cout << "Testing parsing of relational operators..." << std::endl;
    
    std::string source = 
        "int main() {\n"
        "    int a = 5;\n"
        "    int b = 10;\n"
        "    \n"
        "    while (a < b) {\n"
        "        a++;\n"
        "    }\n"
        "    \n"
        "    while (b > a) {\n"
        "        b--;\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}\n";
    
    std::string filename = createParserTestFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    bool success = parser.parse();
    assert(success);
    assert(errorReporter.getErrorCount() == 0);
    
    std::cout << "Relational operators parsing test passed!" << std::endl;
}

// Test parsing of nested expressions
void testParseNestedExpressions() {
    std::cout << "Testing parsing of nested expressions..." << std::endl;
    
    std::string source = 
        "int main() {\n"
        "    int a = 5;\n"
        "    int b = 10;\n"
        "    int c = 15;\n"
        "    int result = 0;\n"
        "    \n"
        "    result = a + b * c;\n"
        "    result = (a + b) * c;\n"
        "    result = a * (b + c / a) - b;\n"
        "    \n"
        "    return 0;\n"
        "}\n";
    
    std::string filename = createParserTestFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    bool success = parser.parse();
    assert(success);
    assert(errorReporter.getErrorCount() == 0);
    
    std::cout << "Nested expressions parsing test passed!" << std::endl;
}

// Test the creation of the parsing table
void testParseTable() {
    std::cout << "Testing parsing table construction..." << std::endl;
    
    // TODO: When parse table is implemented, add assertions here
    
    std::cout << "Parsing table test passed!" << std::endl;
}

// Main function for the test executable
void testParser() {
    std::cout << "Parser tests are prepared but not fully implemented yet.\n";
    std::cout << "To run parser tests, implement the missing functions in src/parser.cpp\n";
    std::cout << "and set PARSER_IMPLEMENTED to true in test_main.cpp\n";
    
    // The following tests will be run when the parser is implemented
    /*
    // Run all the test functions
    testFirstSets();
    testFollowSets();
    testParseTable();
    testParseSimpleProgram();
    testParseWhileLoop();
    testParseArithmeticOperators();
    testParseRelationalOperators();
    testParseNestedExpressions();
    testParseErrors();
    testErrorRecovery();
    
    std::cout << "All parser tests passed!\n";
    */
}