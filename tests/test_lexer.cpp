#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include "lexer.h"
#include "token.h"
#include "error.h"

// Helper function to print token info for debugging
void printTokenInfo(const Token& token) {
    std::cout << "Token: '" << token.lexeme << "' | ";
    
    switch (token.type) {
        case TokenType::Keyword:
            std::cout << "Type: Keyword";
            break;
        case TokenType::Identifier:
            std::cout << "Type: Identifier";
            break;
        case TokenType::IntegerLiteral:
            std::cout << "Type: IntegerLiteral";
            break;
        case TokenType::FloatLiteral:
            std::cout << "Type: FloatLiteral";
            break;
        case TokenType::StringLiteral:
            std::cout << "Type: StringLiteral";
            break;
        case TokenType::Operator:
            std::cout << "Type: Operator";
            break;
        case TokenType::Punctuation:
            std::cout << "Type: Punctuation";
            break;
        case TokenType::Eof:
            std::cout << "Type: EOF";
            break;
        case TokenType::Error:
            std::cout << "Type: Error";
            break;
        default:
            std::cout << "Type: Unknown";
    }
    
    std::cout << " | Line: " << token.loc.line << ", Column: " << token.loc.column << std::endl;
}

// Helper function to create a temporary file with the given source code
std::string createTempFile(const std::string& source) {
    std::string tempFilename = "temp_test_source.c";
    std::ofstream file(tempFilename);
    file << source;
    file.close();
    return tempFilename;
}

// Test that the lexer correctly identifies keywords
void testKeywords() {
    std::string source = "int float while if else return";
    std::string filename = createTempFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // Check that we got 6 keyword tokens + EOF
    assert(!tokenStream.isAtEnd());
    
    Token& t1 = tokenStream.advance();
    assert(t1.type == TokenType::Keyword);
    assert(t1.subtype.keyword == KeywordType::Int);
    assert(t1.lexeme == "int");
    
    Token& t2 = tokenStream.advance();
    assert(t2.type == TokenType::Keyword);
    assert(t2.subtype.keyword == KeywordType::Float);
    assert(t2.lexeme == "float");
    
    Token& t3 = tokenStream.advance();
    assert(t3.type == TokenType::Keyword);
    assert(t3.subtype.keyword == KeywordType::While);
    assert(t3.lexeme == "while");
    
    Token& t4 = tokenStream.advance();
    assert(t4.type == TokenType::Keyword);
    assert(t4.subtype.keyword == KeywordType::If);
    assert(t4.lexeme == "if");
    
    Token& t5 = tokenStream.advance();
    assert(t5.type == TokenType::Keyword);
    assert(t5.subtype.keyword == KeywordType::Else);
    assert(t5.lexeme == "else");
    
    Token& t6 = tokenStream.advance();
    assert(t6.type == TokenType::Keyword);
    assert(t6.subtype.keyword == KeywordType::Return);
    assert(t6.lexeme == "return");
    
    Token& tEof = tokenStream.advance();
    assert(tEof.type == TokenType::Eof);
    
    std::cout << "Keyword test passed!\n";
}

// Test that the lexer correctly identifies identifiers
void testIdentifiers() {
    std::string source = "identifier _identifier123 x y z main";
    std::string filename = createTempFile(source);
    
    std::cout << "Created test file with content: " << source << std::endl;
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // Check that we got tokens + EOF
    assert(!tokenStream.isAtEnd());
    
    std::cout << "Analyzing tokens:" << std::endl;
    
    // First token
    Token& t1 = tokenStream.advance();
    printTokenInfo(t1);
    assert(t1.type == TokenType::Identifier);
    assert(t1.lexeme == "identifier");
    
    // Second token
    Token& t2 = tokenStream.advance();
    printTokenInfo(t2);
    assert(t2.type == TokenType::Identifier);
    assert(t2.lexeme == "_identifier123");
    
    // Third token
    Token& t3 = tokenStream.advance();
    printTokenInfo(t3);
    assert(t3.type == TokenType::Identifier);
    assert(t3.lexeme == "x");
    
    // Fourth token
    Token& t4 = tokenStream.advance();
    printTokenInfo(t4);
    assert(t4.type == TokenType::Identifier);
    assert(t4.lexeme == "y");
    
    // Fifth token
    Token& t5 = tokenStream.advance();
    printTokenInfo(t5);
    assert(t5.type == TokenType::Identifier);
    assert(t5.lexeme == "z");
    
    // Sixth token
    Token& t6 = tokenStream.advance();
    printTokenInfo(t6);
    assert(t6.type == TokenType::Identifier);
    assert(t6.lexeme == "main");
    
    // EOF token
    Token& tEof = tokenStream.advance();
    printTokenInfo(tEof);
    assert(tEof.type == TokenType::Eof);
    
    std::cout << "Identifier test passed!\n";
}

// Test that the lexer correctly identifies literals
void testLiterals() {
    std::string source = "123 456.789 \"string literal\"";
    std::string filename = createTempFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // Integer literal
    Token& t1 = tokenStream.advance();
    assert(t1.type == TokenType::IntegerLiteral);
    assert(t1.value.int_value == 123);
    assert(t1.lexeme == "123");
    
    // Float literal
    Token& t2 = tokenStream.advance();
    assert(t2.type == TokenType::FloatLiteral);
    assert(t2.value.float_value == 456.789f);
    assert(t2.lexeme == "456.789");
    
    // String literal
    Token& t3 = tokenStream.advance();
    assert(t3.type == TokenType::StringLiteral);
    assert(std::string(t3.value.string_value) == "string literal");
    
    std::cout << "Literal test passed!\n";
}

// Test that the lexer correctly identifies operators
void testOperators() {
    std::string source = "+ - * / ++ -- += -= == != < > <= >= = && ||";
    std::string filename = createTempFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // Plus
    Token& t1 = tokenStream.advance();
    assert(t1.type == TokenType::Operator);
    assert(t1.subtype.op == OperatorType::PLUS);
    
    // Minus
    Token& t2 = tokenStream.advance();
    assert(t2.type == TokenType::Operator);
    assert(t2.subtype.op == OperatorType::MINUS);
    
    // Star (multiply)
    Token& t3 = tokenStream.advance();
    assert(t3.type == TokenType::Operator);
    assert(t3.subtype.op == OperatorType::STAR);
    
    // Slash (divide)
    Token& t4 = tokenStream.advance();
    assert(t4.type == TokenType::Operator);
    assert(t4.subtype.op == OperatorType::SLASH);
    
    // Increment
    Token& t5 = tokenStream.advance();
    assert(t5.type == TokenType::Operator);
    assert(t5.subtype.op == OperatorType::INC);
    
    // Decrement
    Token& t6 = tokenStream.advance();
    assert(t6.type == TokenType::Operator);
    assert(t6.subtype.op == OperatorType::DEC);
    
    // Add-assign
    Token& t7 = tokenStream.advance();
    assert(t7.type == TokenType::Operator);
    assert(t7.subtype.op == OperatorType::ADD_ASSIGN);
    
    // Sub-assign
    Token& t8 = tokenStream.advance();
    assert(t8.type == TokenType::Operator);
    assert(t8.subtype.op == OperatorType::SUB_ASSIGN);
    
    // Equal-to
    Token& t9 = tokenStream.advance();
    assert(t9.type == TokenType::Operator);
    assert(t9.subtype.op == OperatorType::EQ);
    
    // Not-equal
    Token& t10 = tokenStream.advance();
    assert(t10.type == TokenType::Operator);
    assert(t10.subtype.op == OperatorType::NE);
    
    // Less-than
    Token& t11 = tokenStream.advance();
    assert(t11.type == TokenType::Operator);
    assert(t11.subtype.op == OperatorType::LESS);
    
    // Greater-than
    Token& t12 = tokenStream.advance();
    assert(t12.type == TokenType::Operator);
    assert(t12.subtype.op == OperatorType::GREATER);
    
    // Less-than-or-equal
    Token& t13 = tokenStream.advance();
    assert(t13.type == TokenType::Operator);
    assert(t13.subtype.op == OperatorType::LE);
    
    // Greater-than-or-equal
    Token& t14 = tokenStream.advance();
    assert(t14.type == TokenType::Operator);
    assert(t14.subtype.op == OperatorType::GE);
    
    // Assignment
    Token& t15 = tokenStream.advance();
    assert(t15.type == TokenType::Operator);
    assert(t15.subtype.op == OperatorType::EQUAL);
    
    // Logical AND
    Token& t16 = tokenStream.advance();
    assert(t16.type == TokenType::Operator);
    assert(t16.subtype.op == OperatorType::AND);
    
    // Logical OR
    Token& t17 = tokenStream.advance();
    assert(t17.type == TokenType::Operator);
    assert(t17.subtype.op == OperatorType::OR);
    
    std::cout << "Operator test passed!\n";
}

// Test that the lexer correctly identifies punctuation
void testPunctuation() {
    std::string source = "{ } ( ) [ ]";
    std::string filename = createTempFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // Left brace
    Token& t1 = tokenStream.advance();
    assert(t1.type == TokenType::Punctuation);
    assert(t1.subtype.punct == PunctuationType::LBRACE);
    
    // Right brace
    Token& t2 = tokenStream.advance();
    assert(t2.type == TokenType::Punctuation);
    assert(t2.subtype.punct == PunctuationType::RBRACE);
    
    // Left parenthesis
    Token& t3 = tokenStream.advance();
    assert(t3.type == TokenType::Punctuation);
    assert(t3.subtype.punct == PunctuationType::LPAREN);
    
    // Right parenthesis
    Token& t4 = tokenStream.advance();
    assert(t4.type == TokenType::Punctuation);
    assert(t4.subtype.punct == PunctuationType::RPAREN);
    
    // Left bracket
    Token& t5 = tokenStream.advance();
    assert(t5.type == TokenType::Punctuation);
    assert(t5.subtype.punct == PunctuationType::LBRACKET);
    
    // Right bracket
    Token& t6 = tokenStream.advance();
    assert(t6.type == TokenType::Punctuation);
    assert(t6.subtype.punct == PunctuationType::RBRACKET);
    
    std::cout << "Punctuation test passed!\n";
}

// Test a complete C function that uses most language features
void testCompleteFunction() {
    std::string source = 
        "int main() {\n"
        "    int i = 0;\n"
        "    float x = 10.5;\n"
        "    \n"
        "    // A while loop\n"
        "    while (i < 10) {\n"
        "        x = x + 1.5;\n"
        "        i++;\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}\n";
    
    std::string filename = createTempFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // Just make sure it parses without errors and count tokens
    int tokenCount = 0;
    while (!tokenStream.isAtEnd()) {
        tokenStream.advance();
        tokenCount++;
    }
    
    // Check that we got a reasonable number of tokens (exclude comments and EOF)
    assert(tokenCount > 20);
    
    std::cout << "Complete function test passed!\n";
}

// Test token location tracking
void testTokenLocation() {
    std::string source = 
        "int main() {\n"
        "    int x = 5;\n"
        "}\n";
    
    std::string filename = createTempFile(source);
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // int (line 1, column 1)
    Token& t1 = tokenStream.advance();
    assert(t1.loc.line == 1);
    assert(t1.loc.column == 1);
    
    // main (line 1, column 5)
    Token& t2 = tokenStream.advance();
    assert(t2.loc.line == 1);
    assert(t2.loc.column == 5);
    
    // ( (line 1, column 9)
    Token& t3 = tokenStream.advance();
    assert(t3.loc.line == 1);
    assert(t3.loc.column == 9);
    
    // ) (line 1, column 10)
    Token& t4 = tokenStream.advance();
    assert(t4.loc.line == 1);
    assert(t4.loc.column == 10);
    
    // { (line 1, column 12)
    Token& t5 = tokenStream.advance();
    assert(t5.loc.line == 1);
    assert(t5.loc.column == 12);
    
    // int (line 2, column 5)
    Token& t6 = tokenStream.advance();
    assert(t6.loc.line == 2);
    assert(t6.loc.column == 5);
    
    std::cout << "Token location test passed!\n";
}

// Test a real sample program from a file
void testSampleProgram() {
    // Use relative path to the test file in the build dir
    std::string filename = "test_files/sample_program.c";
    
    // Check if file exists
    std::ifstream file(filename);
    if (!file.good()) {
        std::cerr << "WARNING: Sample program file not found at: " << filename << std::endl;
        std::cerr << "Creating a temporary file with sample program instead." << std::endl;
        
        // Create a sample program on the fly
        std::string source = 
            "// Sample program for testing\n"
            "main() {\n"
            "    int count = 0;\n"
            "    int sum = 0;\n"
            "    float average = 0.0;\n"
            "    int max = 10;\n"
            "    while (count < max) {\n"
            "        sum = sum + count;\n"
            "        count++;\n"
            "        int square = count * count;\n"
            "    }\n"
            "    average = sum / max;\n"
            "}\n";
        
        filename = createTempFile(source);
    } else {
        file.close();
        std::cout << "Using sample program from: " << filename << std::endl;
    }
    
    errorReporter.init(filename);
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // Count tokens of each type
    int identifiers = 0;
    int keywords = 0;
    int operators = 0;
    int literals = 0;
    int punctuation = 0;
    
    while (!tokenStream.isAtEnd()) {
        Token& token = tokenStream.advance();
        switch (token.type) {
            case TokenType::Identifier:
                identifiers++;
                break;
            case TokenType::Keyword:
                keywords++;
                break;
            case TokenType::Operator:
                operators++;
                break;
            case TokenType::IntegerLiteral:
            case TokenType::FloatLiteral:
            case TokenType::StringLiteral:
                literals++;
                break;
            case TokenType::Punctuation:
                punctuation++;
                break;
            default:
                break;
        }
    }
    
    std::cout << "Token count:" << std::endl;
    std::cout << "  Identifiers: " << identifiers << std::endl;
    std::cout << "  Keywords: " << keywords << std::endl;
    std::cout << "  Operators: " << operators << std::endl;
    std::cout << "  Literals: " << literals << std::endl;
    std::cout << "  Punctuation: " << punctuation << std::endl;
    
    // Verify token counts (these should match what's in the file)
    assert(identifiers >= 6); // count, sum, average, max, square, main
    assert(keywords >= 6);    // int, float, while
    assert(operators >= 10);  // =, <, +, ++, /, etc.
    assert(literals >= 4);    // 0, 0.0, 10
    assert(punctuation >= 8); // (, ), {, }
    
    std::cout << "Sample program test passed!\n";
}

// Main function for the test executable
int main(int argc, char* argv[]) {
    // If a specific test suite is requested, run only that
    if (argc > 1) {
        std::string testSuite = argv[1];
        
        if (testSuite == "lexer") {
            std::cout << "Running lexer tests...\n";
            
            // Run lexer test functions
            testKeywords();
            testIdentifiers();
            testLiterals();
            testOperators();
            testPunctuation();
            testCompleteFunction();
            testTokenLocation();
            testSampleProgram();
            
            std::cout << "All lexer tests passed!\n";
            return 0;
        }
        
        if (testSuite == "parser") {
            std::cout << "Parser tests not implemented yet\n";
            return 0;
        }
        
        if (testSuite == "symbol_table") {
            std::cout << "Symbol table tests not implemented yet\n";
            return 0;
        }
        
        std::cout << "Unknown test suite: " << testSuite << std::endl;
        return 1;
    }
    
    // If no specific suite is requested, run all tests
    std::cout << "Running lexer tests...\n";
    
    // Run test functions
    testKeywords();
    testIdentifiers();
    testLiterals();
    testOperators();
    testPunctuation();
    testCompleteFunction();
    testTokenLocation();
    testSampleProgram();
    
    std::cout << "All lexer tests passed!\n";
    
    std::cout << "\nParser and symbol table tests not implemented yet\n";
    
    return 0;
}