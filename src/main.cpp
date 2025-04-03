#include "lexer.h"
#include "token.h"
#include "error.h"
#include <iostream>
#include <fstream>
#include <string>

void printToken(const Token& token) {
    std::cout << "Token: " << token.lexeme << " | ";
    
    switch (token.type) {
        case TokenType::Keyword:
            std::cout << "Type: Keyword";
            break;
        case TokenType::Identifier:
            std::cout << "Type: Identifier";
            break;
        case TokenType::IntegerLiteral:
            std::cout << "Type: IntegerLiteral, Value: " << token.value.int_value;
            break;
        case TokenType::FloatLiteral:
            std::cout << "Type: FloatLiteral, Value: " << token.value.float_value;
            break;
        case TokenType::StringLiteral:
            std::cout << "Type: StringLiteral, Value: " << token.value.string_value;
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
    }
    
    std::cout << " | Line: " << token.loc.line << ", Column: " << token.loc.column << std::endl;
}

// Simple function to create a test source file
void createTestFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to create test file" << std::endl;
        return;
    }
    
    // Write a simple C program with some of the language features and errors
    file << "// This is a test program with errors\n";
    file << "int main() {\n";
    file << "    int i = 0;\n";
    file << "    float x = 10.5;\n";
    file << "    char* str = \"unterminated string;\n";  // Missing closing quote
    file << "    \n";
    file << "    // Invalid character and syntax errors\n";
    file << "    while (i < 10) {\n";
    file << "        x = x + @1.5;\n";  // Invalid character @
    file << "        i++++;\n";         // Double increment error
    file << "    }\n";
    file << "    \n";
    file << "    // Mismatched brackets and missing semicolon\n";
    file << "    if (x > 20) {\n";
    file << "        return 1\n";       // Missing semicolon
    file << "    \n";                   // Missing closing brace
    file << "    return 0;\n";
    file << "}\n";
    file.close();
    std::cout << "Test file created: " << filename << std::endl;
}

int main(int argc, char* argv[]) {
    std::string filename;
    
    
    filename = "test_program.c";
    createTestFile(filename);
    
    errorReporter.init(filename);
    
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    std::cout << "Tokens in " << filename << ":" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    while (!tokenStream.isAtEnd()) {
        Token& token = tokenStream.peek();
        printToken(token);
        tokenStream.advance();
    }
    
    // Reset and reuse the token stream
    tokenStream.reset();
    
    // Count number of identifiers and keywords
    int identifiers = 0, keywords = 0;
    while (!tokenStream.isAtEnd()) {
        Token& token = tokenStream.advance();
        if (token.type == TokenType::Identifier) {
            identifiers++;
        } else if (token.type == TokenType::Keyword) {
            keywords++;
        }
    }
    
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Statistics:" << std::endl;
    std::cout << "Identifiers: " << identifiers << std::endl;
    std::cout << "Keywords: " << keywords << std::endl;
    std::cout << "Errors: " << errorReporter.getErrorCount() << std::endl;
    
    return 0;
}