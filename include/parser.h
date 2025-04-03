#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "lexer.h"
#include "error.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <variant>

// Non-terminal symbols in our grammar
enum class NonTerminal {
    PROGRAM,
    MAIN_FUNCTION,
    STATEMENT_LIST,
    STATEMENT,
    DECLARATION,
    TYPE,
    ASSIGNMENT,
    LOOP,
    CONDITION,
    EXPRESSION,
    TERM,
    FACTOR,
    RELATIONAL_OP
};

// Class to maintain first and follow sets
class FirstFollowSets {
private:
    std::unordered_map<NonTerminal, std::unordered_set<TokenType>> first_sets;
    std::unordered_map<NonTerminal, std::unordered_set<TokenType>> follow_sets;
    
public:
    FirstFollowSets();
    
    // Initialize the FIRST sets for all non-terminals
    void computeFirstSets();
    
    // Initialize the FOLLOW sets for all non-terminals
    void computeFollowSets();
    
    // Get the FIRST set for a non-terminal
    const std::unordered_set<TokenType>& getFirstSet(NonTerminal nt) const;
    
    // Get the FOLLOW set for a non-terminal
    const std::unordered_set<TokenType>& getFollowSet(NonTerminal nt) const;
    
    // Helper method for debugging
    void printSets() const;
};

// Parsing table entry type
struct ParseTableEntry {
    bool isError;
    std::vector<std::variant<NonTerminal, TokenType>> production;
};

// Top-down parser for Mini-C
class Parser {
private:
    TokenStream tokens;
    ErrorReporter& error_reporter;
    FirstFollowSets first_follow;
    
    // LL(1) parsing table
    std::unordered_map<NonTerminal, std::unordered_map<TokenType, ParseTableEntry>> parse_table;
    
    // Construct the parsing table
    void buildParseTable();
    
    // Parse functions for different non-terminals
    void parseProgram();
    void parseMainFunction();
    void parseStatementList();
    void parseStatement();
    void parseDeclaration();
    void parseType();
    void parseAssignment();
    void parseLoop();
    void parseCondition();
    void parseExpression();
    void parseTerm();
    void parseFactor();
    void parseRelationalOp();
    
    // Error recovery
    void synchronize(const std::unordered_set<TokenType>& follow_set);
    
public:
    Parser(TokenStream tokens, ErrorReporter& reporter);
    
    // Parse the entire program
    bool parse();
    
    // For testing purposes
    const FirstFollowSets& getFirstFollowSets() const;
};

#endif // PARSER_H
