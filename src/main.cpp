#include "lexer.h"
#include "token.h"
#include "error.h"
#include "parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// Options struct to store command line flags
struct Options {
    bool show_tokens = false;
    bool show_parse_table = false;
    bool show_parse_steps = false;
    bool verbose = false;
    std::string input_file = "";
};

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
    
    // Write a syntactically correct C program for parsing
    file << "// This is a test program\n";
    file << "int main() {\n";
    file << "    int i = 0;\n";
    file << "    float x = 10.5;\n";
    file << "    \n";
    file << "    // Loop example\n";
    file << "    while (i < 10) {\n";
    file << "        x = x + 1.5;\n";
    file << "        i++;\n";
    file << "    }\n";
    file << "    \n";
    file << "    return 0;\n";
    file << "}\n";
    file.close();
    std::cout << "Test file created: " << filename << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] [input_file]\n"
              << "Options:\n"
              << "  --show-tokens       Display lexical tokens\n"
              << "  --show-parse-table  Display the LL(1) parse table\n"
              << "  --show-parse-steps  Show detailed parsing steps\n"
              << "  --verbose           Enable verbose output for all stages\n" 
              << "  --help              Display this help message\n"
              << std::endl;
}

// Parse command line arguments
Options parseCommandLine(int argc, char* argv[]) {
    Options options;
    std::vector<std::string> args(argv + 1, argv + argc);
    
    for (const auto& arg : args) {
        if (arg == "--show-tokens") {
            options.show_tokens = true;
        } else if (arg == "--show-parse-table") {
            options.show_parse_table = true;
        } else if (arg == "--show-parse-steps") {
            options.show_parse_steps = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg[0] != '-') {
            // Assume it's the input file
            options.input_file = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            exit(1);
        }
    }
    
    return options;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Options options = parseCommandLine(argc, argv);
    
    // If verbose is set, enable all other options
    if (options.verbose) {
        options.show_tokens = true;
        options.show_parse_table = true;
        options.show_parse_steps = true;
    }
    
    std::string filename;
    
    if (!options.input_file.empty()) {
        filename = options.input_file;
        std::cout << "Using file: " << filename << std::endl;
    } else {
        filename = "test_program.c";
        createTestFile(filename);
    }
    
    errorReporter.init(filename);
    
    // Lexical analysis
    if (options.show_tokens || options.verbose) {
        std::cout << "\n=== LEXICAL ANALYSIS ===\n" << std::endl;
    }
    
    Lexer lexer(filename);
    TokenStream tokenStream = lexer.tokenize();
    
    // Store a copy of the token stream for the parser
    TokenStream parserTokens = tokenStream;
    
    if (options.show_tokens) {
        std::cout << "Tokens in " << filename << ":" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        while (!tokenStream.isAtEnd()) {
            Token& token = tokenStream.peek();
            printToken(token);
            tokenStream.advance();
        }
        
        // Reset token stream for statistics
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
    }
    
    // Only proceed to parsing if there are no lexical errors
    if (errorReporter.getErrorCount() == 0) {
        std::cout << "\n=== SYNTAX ANALYSIS ===\n" << std::endl;
        
        // Create the parser
        Parser parser(parserTokens, errorReporter);
        
        // Enable verbose mode for detailed output if specified
        parser.setVerbose(options.show_parse_steps);
        
        // Display FIRST and FOLLOW sets if verbose
        if (options.verbose) {
            std::cout << "Computing FIRST and FOLLOW sets..." << std::endl;
            const FirstFollowSets& sets = parser.getFirstFollowSets();
            sets.printSets();
        }
        
        // Display parsing table if requested
        if (options.show_parse_table) {
            std::cout << "\nLL(1) Parsing Table:" << std::endl;
            parser.printParseTable();
        }
        
        // Start parsing
        std::cout << "\nStarting LL(1) Parsing..." << std::endl;
        bool parseSuccess = parser.parse();
        
        // Check result
        if (parseSuccess) {
            std::cout << "\nParsing completed successfully." << std::endl;
        } else {
            std::cout << "\nParsing failed with " << errorReporter.getErrorCount() 
                      << " syntax errors." << std::endl;
        }
    } else {
        std::cout << "\nSkipping parsing due to lexical errors." << std::endl;
    }
    
    return 0;
}