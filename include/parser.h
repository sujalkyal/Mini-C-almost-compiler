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
#include <map>
#include <set>
#include <stack>

// Non-terminal symbols in our grammar
enum class NonTerminal {
    PROGRAM,
    MAIN_FUNCTION,
    STATEMENT_LIST,
    STATEMENT,
    DECLARATION,
    DECLARATION_TAIL,
    TYPE,
    ASSIGNMENT,
    LOOP,
    CONDITION,
    RELATIONAL_OP,
    RETURN_STMT,
    EXPRESSION,
    EXPRESSION_TAIL,
    TERM,
    TERM_TAIL,
    FACTOR,
    FACTOR_TAIL
};

// String representation for NonTerminals (for debugging)
inline std::string nonTerminalToString(NonTerminal nt) {
    switch (nt) {
        case NonTerminal::PROGRAM: return "PROGRAM";
        case NonTerminal::MAIN_FUNCTION: return "MAIN_FUNCTION";
        case NonTerminal::STATEMENT_LIST: return "STATEMENT_LIST";
        case NonTerminal::STATEMENT: return "STATEMENT";
        case NonTerminal::DECLARATION: return "DECLARATION";
        case NonTerminal::DECLARATION_TAIL: return "DECLARATION_TAIL";
        case NonTerminal::TYPE: return "TYPE";
        case NonTerminal::ASSIGNMENT: return "ASSIGNMENT";
        case NonTerminal::LOOP: return "LOOP";
        case NonTerminal::CONDITION: return "CONDITION";
        case NonTerminal::RELATIONAL_OP: return "RELATIONAL_OP";
        case NonTerminal::RETURN_STMT: return "RETURN_STMT";
        case NonTerminal::EXPRESSION: return "EXPRESSION";
        case NonTerminal::EXPRESSION_TAIL: return "EXPRESSION_TAIL";
        case NonTerminal::TERM: return "TERM";
        case NonTerminal::TERM_TAIL: return "TERM_TAIL";
        case NonTerminal::FACTOR: return "FACTOR";
        case NonTerminal::FACTOR_TAIL: return "FACTOR_TAIL";
        default: return "UNKNOWN";
    }
}

// Production rule representation A -> aBc | e
struct Production {
    NonTerminal lhs;
    std::vector<std::variant<NonTerminal, std::string, TokenType>> rhs;
    
    // Constructor for creating a production rule
    Production(NonTerminal left, std::vector<std::variant<NonTerminal, std::string, TokenType>> right)
        : lhs(left), rhs(right) {}
};

// Special epsilon symbol
const std::string EPSILON = "ε";

// Class to maintain first and follow sets
class FirstFollowSets {
public:
    std::vector<Production> grammar; // Make grammar accessible to Parser
    std::map<NonTerminal, std::set<std::string>> first_sets;
    std::map<NonTerminal, std::set<std::string>> follow_sets;
    
    FirstFollowSets();
    
    // Add production to grammar
    void addProduction(const Production& prod);
    
    // Initialize the grammar with our language rules
    void initializeGrammar();
    
    // Initialize the FIRST sets for all non-terminals
    void computeFirstSets();
    
    // Calculate FIRST for a specific symbol (terminal, non-terminal, or token type)
    std::set<std::string> calculateFirst(const std::variant<NonTerminal, std::string, TokenType>& symbol);
    
    // Initialize the FOLLOW sets for all non-terminals
    void computeFollowSets();
    
    // Get FIRST for a sequence of symbols
    std::set<std::string> getFirstOfSequence(const std::vector<std::variant<NonTerminal, std::string, TokenType>>& sequence);
    
    // Check if a FIRST set contains epsilon
    bool hasEpsilon(const std::set<std::string>& set) const;
    
    // Get string representation of a token type
    std::string getTokenTypeString(TokenType type) const;
    
    // Helper method for debugging
    void printSets() const;
};

// Parsing table entry type
struct ParseTableEntry {
    int production_index;
    const Production* production;
};

// Top-down parser for Mini-C
class Parser {
private:
    TokenStream tokens;
    ErrorReporter& error_reporter;
    FirstFollowSets first_follow;
    Token* current_token;
    bool verbose; // Control debugging output
    
    // Productions for grammar initialization
    std::vector<Production> productions;
    
    // LL(1) parsing table
    std::map<NonTerminal, std::map<std::string, ParseTableEntry>> parse_table;
    
    // Helper function to get token representation for parsing table
    std::string getTokenKey(const Token& token);
    
    // Check if a token matches expected type or lexeme
    bool matchToken(const std::variant<std::string, TokenType>& expected);
    
    // Error reporting
    void syntaxError(const std::string& message);
    void reportParseError(NonTerminal nonterm);
    
    // Initialize grammar with productions
    void initGrammar();

public:
    Parser(TokenStream tokens, ErrorReporter& reporter);
    
    // Build the parsing table
    void buildParseTable();
    
    // Parse the entire program
    bool parse();
    
    // For testing purposes
    const FirstFollowSets& getFirstFollowSets() const;
    
    // Print parsing table (for debugging)
    void printParseTable() const;
    
    // Enable or disable verbose debugging output
    void setVerbose(bool enable);
};

#endif // PARSER_H
