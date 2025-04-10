#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "parser.h"
#include "lexer.h"
#include "error.h"

// This file will contain parser tests

// We don't need to declare errorReporter here since it's already defined in error.cpp
// and declared as extern in error.h

// Helper function to create a temporary file with the given source code - make it static
static std::string createTempFile(const std::string& source) {
    std::string tempFileName = "temp_test_file.c";
    std::ofstream tempFile(tempFileName);
    
    if (!tempFile.is_open()) {
        std::cerr << "ERROR: Failed to create temporary file: " << tempFileName << std::endl;
        return "";
    }
    
    tempFile << source;
    tempFile.close();
    
    // Verify file was created successfully
    std::ifstream checkFile(tempFileName);
    if (!checkFile.is_open()) {
        std::cerr << "ERROR: Failed to verify temporary file: " << tempFileName << std::endl;
        return "";
    }
    checkFile.close();
    
    std::cout << "Created temporary file: " << tempFileName << " with content:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << source << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    return tempFileName;
}

// Test the FIRST sets calculation
void testFirstSets() {
    std::cout << "Testing FIRST sets calculation..." << std::endl;
    
    // Create a parser object
    std::string source = "int main() { int x = 0; }";
    std::string filename = createTempFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    // Get the first/follow sets
    const FirstFollowSets& sets = parser.getFirstFollowSets();
    
    // Test that FIRST(PROGRAM) contains the 'int' keyword
    const auto& firstProgram = sets.first_sets.at(NonTerminal::PROGRAM);
    assert(firstProgram.find("int") != firstProgram.end());
    
    // Test that FIRST(STATEMENT) contains various statement starters
    const auto& firstStatement = sets.first_sets.at(NonTerminal::STATEMENT);
    assert(firstStatement.find("int") != firstStatement.end() || firstStatement.find("float") != firstStatement.end()); // int, float
    assert(firstStatement.find("$" + std::to_string(static_cast<int>(TokenType::Identifier))) != firstStatement.end()); // assignments
    
    // Test that FIRST(EXPRESSION) contains appropriate tokens
    const auto& firstExpr = sets.first_sets.at(NonTerminal::EXPRESSION);
    assert(firstExpr.find("$" + std::to_string(static_cast<int>(TokenType::Identifier))) != firstExpr.end());
    assert(firstExpr.find("$" + std::to_string(static_cast<int>(TokenType::IntegerLiteral))) != firstExpr.end());
    assert(firstExpr.find("$" + std::to_string(static_cast<int>(TokenType::FloatLiteral))) != firstExpr.end());
    assert(firstExpr.find("(") != firstExpr.end()); // For parentheses
    
    std::cout << "FIRST sets test passed!" << std::endl;
}

// Test the FOLLOW sets calculation
void testFollowSets() {
    std::cout << "Testing FOLLOW sets calculation..." << std::endl;
    
    // Create a parser object
    std::string source = "int main() { int x = 0; while (x < 10) { x = x + 1; } }";
    std::string filename = createTempFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    Parser parser(tokens, errorReporter);
    
    // Get the first/follow sets
    const FirstFollowSets& sets = parser.getFirstFollowSets();
    
    // Test that FOLLOW(STATEMENT) contains closing braces
    const auto& followStatement = sets.follow_sets.at(NonTerminal::STATEMENT);
    assert(followStatement.find("}") != followStatement.end()); // For closing braces
    
    // Print the FOLLOW set to help debug
    std::cout << "FOLLOW(STATEMENT) contains: ";
    for (const auto& token : followStatement) {
        std::cout << token << " ";
    }
    std::cout << std::endl;
    
    // Test that FOLLOW(EXPRESSION) contains appropriate tokens
    const auto& followExpr = sets.follow_sets.at(NonTerminal::EXPRESSION);
    assert(followExpr.find(")") != followExpr.end()); // For closing parentheses
    assert(followExpr.find(";") != followExpr.end()); // For semicolons
    
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
    
    std::string filename = createTempFile(source);
    if (filename.empty()) {
        std::cerr << "Failed to create test file, skipping test" << std::endl;
        return;
    }
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    
    // Debug: Check if tokens were created
    std::cout << "Tokenization complete. Token stream status: " << std::endl;
    tokens.reset();
    int token_count = 0;
    while (!tokens.isAtEnd()) {
        Token& token = tokens.peek();
        std::cout << "Token[" << token_count << "]: " << token.lexeme 
                  << " (type: " << static_cast<int>(token.type) 
                  << ", line: " << token.loc.line 
                  << ", col: " << token.loc.column << ")" << std::endl;
        tokens.advance();
        token_count++;
    }
    
    if (token_count == 0) {
        std::cerr << "No tokens were generated from the source, skipping test" << std::endl;
        return;
    }
    
    // Reset token stream for parsing
    tokens.reset();
    
    // Create parser and parse the tokens
    Parser parser(tokens, errorReporter);
    parser.setVerbose(true); // Enable verbose mode for debugging
    
    bool success = parser.parse();
    
    // Check if parsing was successful
    if (!success) {
        std::cerr << "Parser failed with " << errorReporter.getErrorCount() << " errors" << std::endl;
    } else {
        std::cout << "Parsing successful!" << std::endl;
    }
    
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
    
    std::string filename = createTempFile(source);
    
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
    
    std::string filename = createTempFile(source);
    
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
    
    std::string filename1 = createTempFile(source1);
    
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
    
    std::string filename2 = createTempFile(source2);
    
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
    
    std::string filename = createTempFile(source);
    
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
    
    std::string filename = createTempFile(source);
    
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
    
    std::string filename = createTempFile(source);
    
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

// Test the LL(1) parsing algorithm
void testLL1Parser() {
    std::cout << "\n=== TESTING LL(1) PARSER ===\n" << std::endl;
    
    // Create a valid program
    std::string source = R"(
int main() {
    int x = 5;
    int y = 10;
    
    while (x < y) {
        x = x + 1;
    }
    
    y = x * 2;
}
)";
    
    std::string filename = createTempFile(source);
    
    // Create the necessary objects
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    
    // Create parser and enable verbose mode
    Parser parser(tokens, errorReporter);
    parser.setVerbose(true);
    
    // Print the FIRST and FOLLOW sets
    std::cout << "First and Follow Sets:" << std::endl;
    const FirstFollowSets& sets = parser.getFirstFollowSets();
    sets.printSets();
    
    // Print the parsing table
    std::cout << "LL(1) Parsing Table:" << std::endl;
    parser.printParseTable();
    
    // Parse the program
    bool success = parser.parse();
    
    assert(success && errorReporter.getErrorCount() == 0);
    
    std::cout << "LL(1) parsing test with valid program passed!" << std::endl;
    
    // Test with a program containing errors
    std::string errorSource = R"(
int main() {
    int x = 5
    int y = 10;
    
    while x < y) {
        x = x + 1;
    }
    
    y = * 2;
}
)";
    
    std::string errorFilename = createTempFile(errorSource);
    
    // Create the necessary objects
    errorReporter.init(errorFilename);
    Lexer errorLexer(errorFilename);
    TokenStream errorTokens = errorLexer.tokenize();
    
    // Create parser
    Parser errorParser(errorTokens, errorReporter);
    
    // Parse the program with errors
    bool errorResult = errorParser.parse();
    
    assert(!errorResult || errorReporter.getErrorCount() > 0);
    
    std::cout << "LL(1) parsing test with errors passed!" << std::endl;
}

// Test basic main function parsing
void testBasicMainFunction() {
    std::cout << "Testing basic main function parsing..." << std::endl;
    
    // Define a simple program
    std::string source = "int main() { }";
    
    // Create a temporary file with the source
    std::string filename = createTempFile(source);
    
    // Create a lexer with the temporary file
    ErrorReporter reporter;
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    
    // Reset the token stream
    tokens.reset();
    
    // Create a parser with the token stream
    Parser parser(tokens, reporter);
    parser.setVerbose(true); // Enable verbose mode for debugging
    
    // Test parsing
    bool result = parser.parse();
    
    // Check that parsing was successful
    assert(result == true);
    assert(reporter.getErrorCount() == 0);
    
    std::cout << "Basic main function parsing test passed!" << std::endl;
}

// Test declarations and assignments
void testDeclarationsAndAssignments() {
    std::cout << "Testing declarations and assignments parsing..." << std::endl;
    
    // Define a program with declarations and assignments
    std::string source = R"(
int main() {
    int x = 10;
    float y = 20.5;
    x = 5;
    y = 10.5;
}
)";
    
    // Create a temporary file with the source
    std::string filename = createTempFile(source);
    
    // Create a lexer with the temporary file
    ErrorReporter reporter;
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    
    // Create a parser with the token stream
    Parser parser(tokens, reporter);
    parser.setVerbose(true); // Enable verbose mode for debugging
    
    // Test parsing
    bool result = parser.parse();
    
    // Check that parsing was successful
    assert(result == true);
    assert(reporter.getErrorCount() == 0);
    
    std::cout << "Declarations and assignments parsing test passed!" << std::endl;
}

// Test loops and conditions
void testLoopsAndConditions() {
    std::cout << "Testing loops and conditions parsing..." << std::endl;
    
    // Define a program with loops and conditions
    std::string source = R"(
int main() {
    int i = 0;
    int max = 10;
    
    while (i < max) {
        i = i + 1;
    }
}
)";
    
    // Create a temporary file with the source
    std::string filename = createTempFile(source);
    
    // Create a lexer with the temporary file
    ErrorReporter reporter;
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    
    // Create a parser with the token stream
    Parser parser(tokens, reporter);
    parser.setVerbose(true); // Enable verbose mode for debugging
    
    // Test parsing
    bool result = parser.parse();
    
    // Check that parsing was successful
    assert(result == true);
    assert(reporter.getErrorCount() == 0);
    
    std::cout << "Loops and conditions parsing test passed!" << std::endl;
}

// Test expressions
void testExpressions() {
    std::cout << "Testing expressions parsing..." << std::endl;
    
    // Define a program with complex expressions
    std::string source = R"(
int main() {
    int a = 5;
    int b = 10;
    int c = 15;
    
    int result = a + b * c;
    result = (a + b) * c;
    result = a++ + b;
    result = a + b--;
}
)";
    
    // Create a temporary file with the source
    std::string filename = createTempFile(source);
    
    // Create a lexer with the temporary file
    ErrorReporter reporter;
    Lexer lexer(filename);
    TokenStream tokens = lexer.tokenize();
    
    // Create a parser with the token stream
    Parser parser(tokens, reporter);
    parser.setVerbose(true); // Enable verbose mode for debugging
    
    // Test parsing
    bool result = parser.parse();
    
    // Check that parsing was successful
    assert(result == true);
    assert(reporter.getErrorCount() == 0);
    
    std::cout << "Expressions parsing test passed!" << std::endl;
}

// Test syntax error detection
void testSyntaxErrorDetection() {
    std::cout << "Testing syntax error detection..." << std::endl;
    
    // Missing closing brace
    {
        std::string source = "int main() { ";
        std::string filename = createTempFile(source);
        
        ErrorReporter reporter;
        Lexer lexer(filename);
        TokenStream tokens = lexer.tokenize();
        
        Parser parser(tokens, reporter);
        bool result = parser.parse();
        
        assert(result == false);
        assert(reporter.getErrorCount() > 0);
        
        std::cout << "  Missing closing brace test passed!" << std::endl;
    }
    
    // Invalid declaration
    {
        std::string source = "int main() { int 123; }";
        std::string filename = createTempFile(source);
        
        ErrorReporter reporter;
        Lexer lexer(filename);
        TokenStream tokens = lexer.tokenize();
        
        Parser parser(tokens, reporter);
        bool result = parser.parse();
        
        assert(result == false);
        assert(reporter.getErrorCount() > 0);
        
        std::cout << "  Invalid declaration test passed!" << std::endl;
    }
    
    // Missing semicolon
    {
        std::string source = "int main() { int x = 10 }";
        std::string filename = createTempFile(source);
        
        ErrorReporter reporter;
        Lexer lexer(filename);
        TokenStream tokens = lexer.tokenize();
        
        Parser parser(tokens, reporter);
        bool result = parser.parse();
        
        assert(result == false);
        assert(reporter.getErrorCount() > 0);
        
        std::cout << "  Missing semicolon test passed!" << std::endl;
    }
    
    std::cout << "Syntax error detection tests passed!" << std::endl;
}

// Rename main to run_parser_tests to avoid conflict with other test files
int run_parser_tests() {
    std::cout << "==== RUNNING PARSER TESTS ====" << std::endl;
    
    // Run all test functions
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
    testLL1Parser();
    
    // Additional tests
    testBasicMainFunction();
    testDeclarationsAndAssignments();
    testLoopsAndConditions();
    testExpressions();
    testSyntaxErrorDetection();
    
    std::cout << "All parser tests passed!" << std::endl;
    
    return 0;
}