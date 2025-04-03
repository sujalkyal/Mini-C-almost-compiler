#include "parser.h"
#include <iostream>
#include <variant>

// FirstFollowSets implementation
FirstFollowSets::FirstFollowSets() {
    // Initialize empty sets
    // This will be fully implemented later
}

void FirstFollowSets::computeFirstSets() {
    // TODO: Implement the algorithm to compute FIRST sets
    // This is a placeholder
}

void FirstFollowSets::computeFollowSets() {
    // TODO: Implement the algorithm to compute FOLLOW sets
    // This is a placeholder
}

const std::unordered_set<TokenType>& FirstFollowSets::getFirstSet(NonTerminal nt) const {
    // Return the FIRST set for a non-terminal
    return first_sets.at(nt);
}

const std::unordered_set<TokenType>& FirstFollowSets::getFollowSet(NonTerminal nt) const {
    // Return the FOLLOW set for a non-terminal
    return follow_sets.at(nt);
}

void FirstFollowSets::printSets() const {
    // Print FIRST and FOLLOW sets for debugging
    std::cout << "FIRST sets:" << std::endl;
    // TODO: Implement printing of FIRST sets

    std::cout << "FOLLOW sets:" << std::endl;
    // TODO: Implement printing of FOLLOW sets
}

// Parser implementation
Parser::Parser(TokenStream tokens, ErrorReporter& reporter)
    : tokens(tokens), error_reporter(reporter) {
    // Initialize parser
    // Compute FIRST and FOLLOW sets
    buildParseTable();
}

void Parser::buildParseTable() {
    // TODO: Implement the construction of the LL(1) parsing table
    // This will use the FIRST and FOLLOW sets
}

bool Parser::parse() {
    // Start the parsing process
    parseProgram();
    
    // Check if we reached the end of the file
    if (tokens.peek().type != TokenType::Eof) {
        // Error: there are unparsed tokens
        Token& t = tokens.peek();
        error_reporter.error(t.loc, "Unexpected token at end of file");
        return false;
    }
    
    return true;
}

void Parser::parseProgram() {
    // TODO: Implement parsing of the program non-terminal
}

void Parser::parseMainFunction() {
    // TODO: Implement parsing of the main function
}

void Parser::parseStatementList() {
    // TODO: Implement parsing of statement lists
}

void Parser::parseStatement() {
    // TODO: Implement parsing of individual statements
}

void Parser::parseDeclaration() {
    // TODO: Implement parsing of declarations
}

void Parser::parseType() {
    // TODO: Implement parsing of types
}

void Parser::parseAssignment() {
    // TODO: Implement parsing of assignments
}

void Parser::parseLoop() {
    // TODO: Implement parsing of while loops
}

void Parser::parseCondition() {
    // TODO: Implement parsing of conditions
}

void Parser::parseExpression() {
    // TODO: Implement parsing of expressions
}

void Parser::parseTerm() {
    // TODO: Implement parsing of terms
}

void Parser::parseFactor() {
    // TODO: Implement parsing of factors
}

void Parser::parseRelationalOp() {
    // TODO: Implement parsing of relational operators
}

void Parser::synchronize(const std::unordered_set<TokenType>& follow_set) {
    // Implement panic mode error recovery
    // Skip tokens until we find one in the FOLLOW set
    while (!tokens.isAtEnd()) {
        if (follow_set.find(tokens.peek().type) != follow_set.end()) {
            break;
        }
        tokens.advance();
    }
}

const FirstFollowSets& Parser::getFirstFollowSets() const {
    return first_follow;
}
