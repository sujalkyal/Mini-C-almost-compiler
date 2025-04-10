#include "parser.h"
#include <iostream>
#include <variant>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Special epsilon symbol
#define EPSILON "ε"

// Define a constant for no production
const int NO_PRODUCTION = -1;

// Helper function for token type string representation
std::string tokenTypeToString(TokenType type) {
    switch(type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::IntegerLiteral: return "IntegerLiteral";
        case TokenType::FloatLiteral: return "FloatLiteral"; 
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::Keyword: return "Keyword";
        case TokenType::Operator: return "Operator";
        case TokenType::Punctuation: return "Punctuation";
        case TokenType::Eof: return "EOF";
        case TokenType::Error: return "Error";
        default: return "Unknown";
    }
}

// Helper function to convert a token to its symbol representation for parsing
std::string getTokenSymbol(const Token& token) {
    if (token.type == TokenType::Eof) {
        return "$"; // Special case for EOF
    }
    return token.lexeme;
}

// FirstFollowSets implementation
FirstFollowSets::FirstFollowSets() {
    // Initialize grammar and compute sets
    initializeGrammar();
    computeFirstSets();
    computeFollowSets();
}

void FirstFollowSets::addProduction(const Production& prod) {
    grammar.push_back(prod);
}

void FirstFollowSets::initializeGrammar() {
    // Initialize our mini-C grammar with production rules based on grammar.txt
    
    // PROGRAM → MAIN_FUNCTION
    addProduction(Production(
        NonTerminal::PROGRAM,
        {NonTerminal::MAIN_FUNCTION}
    ));
    
    // MAIN_FUNCTION → int main ( ) { STATEMENT_LIST }
    addProduction(Production(
        NonTerminal::MAIN_FUNCTION,
        {"int", "main", "(", ")", "{", NonTerminal::STATEMENT_LIST, "}"}
    ));
    
    // STATEMENT_LIST → STATEMENT STATEMENT_LIST | ε
    addProduction(Production(
        NonTerminal::STATEMENT_LIST,
        {NonTerminal::STATEMENT, NonTerminal::STATEMENT_LIST}
    ));
    
    addProduction(Production(
        NonTerminal::STATEMENT_LIST,
        {EPSILON} // Empty production (epsilon)
    ));
    
    // STATEMENT → DECLARATION | ASSIGNMENT | LOOP | RETURN_STMT | EXPRESSION ; | ε
    addProduction(Production(
        NonTerminal::STATEMENT,
        {NonTerminal::DECLARATION}
    ));
    
    addProduction(Production(
        NonTerminal::STATEMENT,
        {NonTerminal::ASSIGNMENT}
    ));
    
    addProduction(Production(
        NonTerminal::STATEMENT,
        {NonTerminal::LOOP}
    ));
    
    addProduction(Production(
        NonTerminal::STATEMENT,
        {NonTerminal::RETURN_STMT}
    ));
    
    // Add support for expression statements (like i++;)
    addProduction(Production(
        NonTerminal::STATEMENT,
        {NonTerminal::EXPRESSION, ";"}
    ));
    
    addProduction(Production(
        NonTerminal::STATEMENT,
        {EPSILON}
    ));
    
    // DECLARATION → TYPE IDENTIFIER DECLARATION_TAIL
    addProduction(Production(
        NonTerminal::DECLARATION,
        {NonTerminal::TYPE, TokenType::Identifier, NonTerminal::DECLARATION_TAIL}
    ));
    
    // DECLARATION_TAIL → = EXPRESSION ; | ;
    addProduction(Production(
        NonTerminal::DECLARATION_TAIL,
        {"=", NonTerminal::EXPRESSION, ";"}
    ));
    
    addProduction(Production(
        NonTerminal::DECLARATION_TAIL,
        {";"}
    ));
    
    // TYPE → int | float
    addProduction(Production(
        NonTerminal::TYPE,
        {"int"}
    ));
    
    addProduction(Production(
        NonTerminal::TYPE,
        {"float"}
    ));
    
    // ASSIGNMENT → IDENTIFIER = EXPRESSION ;
    addProduction(Production(
        NonTerminal::ASSIGNMENT,
        {TokenType::Identifier, "=", NonTerminal::EXPRESSION, ";"}
    ));
    
    // LOOP → while ( CONDITION ) { STATEMENT_LIST }
    addProduction(Production(
        NonTerminal::LOOP,
        {"while", "(", NonTerminal::CONDITION, ")", "{", NonTerminal::STATEMENT_LIST, "}"}
    ));
    
    // CONDITION → EXPRESSION RELATIONAL_OP EXPRESSION
    addProduction(Production(
        NonTerminal::CONDITION,
        {NonTerminal::EXPRESSION, NonTerminal::RELATIONAL_OP, NonTerminal::EXPRESSION}
    ));
    
    // RELATIONAL_OP → < | > | <= | >= | == | !=
    addProduction(Production(
        NonTerminal::RELATIONAL_OP,
        {"<"}
    ));
    
    addProduction(Production(
        NonTerminal::RELATIONAL_OP,
        {">"}
    ));
    
    addProduction(Production(
        NonTerminal::RELATIONAL_OP,
        {"<="}
    ));
    
    addProduction(Production(
        NonTerminal::RELATIONAL_OP,
        {">="}
    ));
    
    addProduction(Production(
        NonTerminal::RELATIONAL_OP,
        {"=="}
    ));
    
    addProduction(Production(
        NonTerminal::RELATIONAL_OP,
        {"!="}
    ));
    
    // RETURN_STMT → return EXPRESSION ;
    addProduction(Production(
        NonTerminal::RETURN_STMT,
        {"return", NonTerminal::EXPRESSION, ";"}
    ));
    
    // EXPRESSION → TERM EXPRESSION_TAIL
    addProduction(Production(
        NonTerminal::EXPRESSION,
        {NonTerminal::TERM, NonTerminal::EXPRESSION_TAIL}
    ));
    
    // EXPRESSION_TAIL → + TERM EXPRESSION_TAIL | - TERM EXPRESSION_TAIL | ε
    addProduction(Production(
        NonTerminal::EXPRESSION_TAIL,
        {"+", NonTerminal::TERM, NonTerminal::EXPRESSION_TAIL}
    ));
    
    addProduction(Production(
        NonTerminal::EXPRESSION_TAIL,
        {"-", NonTerminal::TERM, NonTerminal::EXPRESSION_TAIL}
    ));
    
    addProduction(Production(
        NonTerminal::EXPRESSION_TAIL,
        {EPSILON}
    ));
    
    // TERM → FACTOR TERM_TAIL
    addProduction(Production(
        NonTerminal::TERM,
        {NonTerminal::FACTOR, NonTerminal::TERM_TAIL}
    ));
    
    // TERM_TAIL → * FACTOR TERM_TAIL | / FACTOR TERM_TAIL | ε
    addProduction(Production(
        NonTerminal::TERM_TAIL,
        {"*", NonTerminal::FACTOR, NonTerminal::TERM_TAIL}
    ));
    
    addProduction(Production(
        NonTerminal::TERM_TAIL,
        {"/", NonTerminal::FACTOR, NonTerminal::TERM_TAIL}
    ));
    
    addProduction(Production(
        NonTerminal::TERM_TAIL,
        {EPSILON}
    ));
    
    // FACTOR → IDENTIFIER FACTOR_TAIL | INTEGER_LITERAL | FLOAT_LITERAL | ( EXPRESSION )
    addProduction(Production(
        NonTerminal::FACTOR,
        {TokenType::Identifier, NonTerminal::FACTOR_TAIL}
    ));
    
    addProduction(Production(
        NonTerminal::FACTOR,
        {TokenType::IntegerLiteral}
    ));
    
    addProduction(Production(
        NonTerminal::FACTOR,
        {TokenType::FloatLiteral}
    ));
    
    addProduction(Production(
        NonTerminal::FACTOR,
        {"(", NonTerminal::EXPRESSION, ")"}
    ));
    
    // FACTOR_TAIL → ++ | -- | ε
    addProduction(Production(
        NonTerminal::FACTOR_TAIL,
        {"++"}
    ));
    
    addProduction(Production(
        NonTerminal::FACTOR_TAIL,
        {"--"}
    ));
    
    addProduction(Production(
        NonTerminal::FACTOR_TAIL,
        {EPSILON}
    ));
}

void FirstFollowSets::computeFirstSets() {
    bool changed = true;
    
    // Initialize FIRST sets for all non-terminals
    for (int i = 0; i <= static_cast<int>(NonTerminal::FACTOR_TAIL); i++) {
        NonTerminal nt = static_cast<NonTerminal>(i);
        first_sets[nt] = std::set<std::string>();
    }
    
    // Add epsilon to FIRST of any non-terminal that can derive epsilon
    for (const auto& production : grammar) {
        if (production.rhs.size() == 1 && 
            std::holds_alternative<std::string>(production.rhs[0]) && 
            std::get<std::string>(production.rhs[0]) == EPSILON) {
            first_sets[production.lhs].insert(EPSILON);
        }
    }
    
    // Compute FIRST sets until no more changes
    while (changed) {
        changed = false;
        
        for (const auto& production : grammar) {
            const NonTerminal& lhs = production.lhs;
            size_t oldSize = first_sets[lhs].size();
            
            // If rhs begins with a terminal, add it to FIRST
            if (!production.rhs.empty()) {
                if (std::holds_alternative<std::string>(production.rhs[0])) {
                    std::string terminal = std::get<std::string>(production.rhs[0]);
                    if (terminal != EPSILON) {
                        first_sets[lhs].insert(terminal);
                    }
                } else if (std::holds_alternative<TokenType>(production.rhs[0])) {
                    TokenType tokenType = std::get<TokenType>(production.rhs[0]);
                    // Add a placeholder for the token type
                    std::string tokenTypeStr = "$" + std::to_string(static_cast<int>(tokenType));
                    first_sets[lhs].insert(tokenTypeStr);
                } else {
                    // If rhs begins with a non-terminal, add its FIRST set (except epsilon)
                    NonTerminal firstNT = std::get<NonTerminal>(production.rhs[0]); 
                    for (const std::string& t : first_sets[firstNT]) {
                        if (t != EPSILON) {
                            first_sets[lhs].insert(t);
                        }
                    }
                    
                    // If all symbols in rhs can derive epsilon, add epsilon to FIRST
                    bool allCanDeriveEpsilon = true;
                    for (const auto& symbol : production.rhs) {
                        if (std::holds_alternative<std::string>(symbol)) {
                            if (std::get<std::string>(symbol) != EPSILON) {
                                allCanDeriveEpsilon = false;
                                break;
                            }
                        } else if (std::holds_alternative<TokenType>(symbol)) {
                            // TokenType cannot derive epsilon
                            allCanDeriveEpsilon = false;
                            break;
                        } else {
                            NonTerminal nt = std::get<NonTerminal>(symbol);
                            if (first_sets[nt].find(EPSILON) == first_sets[nt].end()) {
                                allCanDeriveEpsilon = false;
                                break;
                            }
                        }
                    }
                    
                    if (allCanDeriveEpsilon) {
                        first_sets[lhs].insert(EPSILON);
                    }
                }
            }
            
            // If the set changed, we need another iteration
            if (first_sets[lhs].size() > oldSize) {
                changed = true;
            }
        }
    }
}

std::set<std::string> FirstFollowSets::calculateFirst(const std::variant<NonTerminal, std::string, TokenType>& symbol) {
    if (std::holds_alternative<std::string>(symbol)) {
        return {std::get<std::string>(symbol)};
    } else if (std::holds_alternative<TokenType>(symbol)) {
        TokenType tokenType = std::get<TokenType>(symbol);
        return {"$" + std::to_string(static_cast<int>(tokenType))};
    } else {
        return first_sets[std::get<NonTerminal>(symbol)];
    }
}

std::set<std::string> FirstFollowSets::getFirstOfSequence( 
    const std::vector<std::variant<NonTerminal, std::string, TokenType>>& sequence) {
    
    std::set<std::string> result;
    
    for (size_t i = 0; i < sequence.size(); i++) {
        const auto& symbol = sequence[i];
        std::set<std::string> firstOfSymbol;
        
        if (std::holds_alternative<std::string>(symbol)) {
            std::string token = std::get<std::string>(symbol);
            if (token != EPSILON) {
                result.insert(token);
                break;  // Non-epsilon terminal, stop here
            }
        } else if (std::holds_alternative<TokenType>(symbol)) {
            TokenType tokenType = std::get<TokenType>(symbol);
            result.insert("$" + std::to_string(static_cast<int>(tokenType)));
            break;  // Terminal, stop here
        } else {
            // For non-terminals, add all tokens from its FIRST set except epsilon
            NonTerminal nt = std::get<NonTerminal>(symbol);
            for (const std::string& t : first_sets[nt]) {
                if (t != EPSILON) {
                    result.insert(t);
                }
            }
            
            // If this non-terminal cannot derive epsilon, stop here
            if (first_sets[nt].find(EPSILON) == first_sets[nt].end()) {
                break;
            }
            
            // If we're at the last symbol and it can derive epsilon, add epsilon to result
            if (i == sequence.size() - 1 && first_sets[nt].find(EPSILON) != first_sets[nt].end()) {
                result.insert(EPSILON);
            }
        }
    }
    
    return result;
}

bool FirstFollowSets::hasEpsilon(const std::set<std::string>& set) const {
    return set.find(EPSILON) != set.end();
}

void FirstFollowSets::computeFollowSets() {
    // Initialize FOLLOW sets for all non-terminals
    for (int i = 0; i <= static_cast<int>(NonTerminal::FACTOR_TAIL); i++) {
        NonTerminal nt = static_cast<NonTerminal>(i);
        follow_sets[nt] = std::set<std::string>();
    }
    
    // Add $ to FOLLOW of start symbol (PROGRAM)
    follow_sets[NonTerminal::PROGRAM].insert("$"); 
    
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& production : grammar) {
            const NonTerminal& lhs = production.lhs;
            
            for (size_t i = 0; i < production.rhs.size(); i++) {
                // Only interested in non-terminals in RHS
                if (!std::holds_alternative<NonTerminal>(production.rhs[i])) {
                    continue;
                }
                
                NonTerminal B = std::get<NonTerminal>(production.rhs[i]);
                size_t oldSize = follow_sets[B].size();
                
                // If B is the last symbol, add FOLLOW(A) to FOLLOW(B)
                if (i == production.rhs.size() - 1) {
                    for (const std::string& t : follow_sets[lhs]) {
                        follow_sets[B].insert(t);
                    }
                } else {
                    // Get FIRST of the sequence after B
                    std::vector<std::variant<NonTerminal, std::string, TokenType>> beta(
                        production.rhs.begin() + i + 1, production.rhs.end());
                    std::set<std::string> firstBeta = getFirstOfSequence(beta);
                    
                    // Add FIRST(beta) - {epsilon} to FOLLOW(B)
                    for (const std::string& t : firstBeta) {
                        if (t != EPSILON) {
                            follow_sets[B].insert(t);
                        }
                    }
                    
                    // If epsilon is in FIRST(beta), add FOLLOW(A) to FOLLOW(B)
                    if (firstBeta.find(EPSILON) != firstBeta.end()) {
                        for (const std::string& t : follow_sets[lhs]) {
                            follow_sets[B].insert(t);
                        }
                    }
                }
                
                // If FOLLOW(B) changed, we need another iteration
                if (follow_sets[B].size() > oldSize) {
                    changed = true;
                }
            }
        }
    }
}

void FirstFollowSets::printSets() const {
    std::cout << "\n==== FIRST SETS ====\n";
    for (int i = 0; i <= static_cast<int>(NonTerminal::FACTOR_TAIL); i++) {
        NonTerminal nt = static_cast<NonTerminal>(i);
        std::cout << "FIRST(" << nonTerminalToString(nt) << ") = { ";
        
        for (auto it = first_sets.at(nt).begin(); it != first_sets.at(nt).end(); ++it) {
            if (it != first_sets.at(nt).begin()) {
                std::cout << ", ";
            }
            std::cout << *it;
        }
        std::cout << " }\n";
    }
    
    std::cout << "\n==== FOLLOW SETS ====\n";
    for (int i = 0; i <= static_cast<int>(NonTerminal::FACTOR_TAIL); i++) {
        NonTerminal nt = static_cast<NonTerminal>(i);
        std::cout << "FOLLOW(" << nonTerminalToString(nt) << ") = { ";
        
        for (auto it = follow_sets.at(nt).begin(); it != follow_sets.at(nt).end(); ++it) {
            if (it != follow_sets.at(nt).begin()) {
                std::cout << ", ";
            }
            std::cout << *it;
        }
        std::cout << " }\n";
    }
}

// Parser implementation
Parser::Parser(TokenStream tokens, ErrorReporter& reporter, SymbolTable& symtable) 
    : tokens(std::move(tokens)), error_reporter(reporter), symbol_table(symtable), first_follow(), parse_table(), current_token(nullptr), verbose(false) {
    this->tokens.reset();  // Reset the token stream to ensure we're at the beginning
    current_token = &this->tokens.peek();
    buildParseTable();
}

void Parser::buildParseTable() {
    // Initialize parse table with error entries
    for (int nt = 0; nt <= static_cast<int>(NonTerminal::FACTOR_TAIL); nt++) {
        for (const auto& terminals_entry : first_follow.first_sets) {
            for (const auto& terminal : terminals_entry.second) {
                if (terminal != EPSILON) {
                    parse_table[static_cast<NonTerminal>(nt)][terminal] = {NO_PRODUCTION, nullptr};
                }
            }
        }
        
        for (const auto& terminal : first_follow.follow_sets[static_cast<NonTerminal>(nt)]) {
            if (terminal != EPSILON) {
                parse_table[static_cast<NonTerminal>(nt)][terminal] = {NO_PRODUCTION, nullptr};
            }
        }
    }
    
    // Helper to check if a conflict is expected and should be suppressed
    auto isExpectedConflict = [](NonTerminal nonterm, const std::string& terminal) -> bool {
        // STATEMENT_LIST can have epsilon or contain statements, so } can cause conflicts
        if (nonterm == NonTerminal::STATEMENT_LIST && terminal == "}") 
            return true;
            
        // STATEMENT can be multiple things, so possible conflicts with these terminals
        if (nonterm == NonTerminal::STATEMENT) {
            if (terminal == "$1" ||  // Identifier
                terminal == "$2" ||  // IntegerLiteral
                terminal == "$3" ||  // FloatLiteral
                terminal == "(" ||   // Open parenthesis
                terminal == "int" || 
                terminal == "float" || 
                terminal == "return" || 
                terminal == "while")
                return true;
        }
        return false;
    };
    
    // Populate parse table based on grammar productions
    for (size_t i = 0; i < first_follow.grammar.size(); i++) {
        const Production& prod = first_follow.grammar[i];
        NonTerminal nonterm = prod.lhs;
        
        // If the production has epsilon (ε) on RHS, add to FOLLOW(nonterm)
        if (prod.rhs.size() == 1 && 
            std::holds_alternative<std::string>(prod.rhs[0]) && 
            std::get<std::string>(prod.rhs[0]) == EPSILON) {
            
            for (const auto& terminal : first_follow.follow_sets[nonterm]) {
                auto& table_entry = parse_table[nonterm][terminal];
                
                // Check for conflicts, but suppress expected conflicts
                if (table_entry.production_index != NO_PRODUCTION && !isExpectedConflict(nonterm, terminal)) {
                    SourceLocation loc = current_token->loc;
                    error_reporter.error(loc, "Parser conflict: Multiple productions for %s with terminal %s", 
                                        nonTerminalToString(nonterm).c_str(), terminal.c_str());
                } else {
                    table_entry = {static_cast<int>(i), &prod};
                }
            }
        } else {
            // For non-epsilon productions, add to FIRST(RHS)
            std::set<std::string> first_rhs = first_follow.getFirstOfSequence(prod.rhs);
            
            for (const std::string& terminal : first_rhs) {
                if (terminal != EPSILON) {
                    auto& table_entry = parse_table[nonterm][terminal];
                    
                    // Check for conflicts, but suppress expected conflicts
                    if (table_entry.production_index != NO_PRODUCTION && !isExpectedConflict(nonterm, terminal)) {
                        SourceLocation loc = current_token->loc;
                        error_reporter.error(loc, "Parser conflict: Multiple productions for %s with terminal %s", 
                                            nonTerminalToString(nonterm).c_str(), terminal.c_str());
                    } else {
                        table_entry = {static_cast<int>(i), &prod};
                    }
                }
            }
            
            // If FIRST(RHS) contains epsilon, add production to FOLLOW(nonterm)
            if (first_follow.hasEpsilon(first_rhs)) {
                for (const auto& terminal : first_follow.follow_sets[nonterm]) {
                    auto& table_entry = parse_table[nonterm][terminal];
                    
                    // Check for conflicts, but suppress expected conflicts
                    if (table_entry.production_index != NO_PRODUCTION && !isExpectedConflict(nonterm, terminal)) {
                        SourceLocation loc = current_token->loc;
                        error_reporter.error(loc, "Parser conflict: Multiple productions for %s with terminal %s",
                                            nonTerminalToString(nonterm).c_str(), terminal.c_str());
                    } else {
                        table_entry = {static_cast<int>(i), &prod};
                    }
                }
            }
        }
    }
}

bool Parser::parse() {
    // Debug output - print the first few tokens
    std::cout << "Token stream status: " << (tokens.isAtEnd() ? "empty" : "has tokens") << std::endl;
    std::cout << "Current token: " << current_token->lexeme << " (type: " << static_cast<int>(current_token->type) << ")" << std::endl;
    
    // Safety check - if we already have errors or no tokens, return false
    if (tokens.isAtEnd() || error_reporter.getErrorCount() > 0) {
        if (tokens.isAtEnd()) {
            std::cout << "Error: Token stream is empty" << std::endl;
        }
        return false;
    }
    
    // Initialize parse stack with EOF marker and start symbol
    std::vector<std::variant<NonTerminal, std::string, TokenType>> parse_stack;
    parse_stack.push_back("$"); // EOF marker at bottom of stack
    parse_stack.push_back(NonTerminal::PROGRAM); // Start symbol
    
    // For symbol table processing
    SymbolType current_type = SymbolType::UNKNOWN;
    std::string current_identifier = "";
    bool processing_declaration = false;
    
    // Create the global scope
    symbol_table.enterScope(); // Start with scope level 0
    
    // To prevent infinite loops, set a maximum iteration count
    int max_iterations = 1000;
    int iterations = 0;
    
    while (!parse_stack.empty() && iterations < max_iterations) {
        iterations++;
        
        // Check what's on top of the stack
        std::variant<NonTerminal, std::string, TokenType> top = parse_stack.back();
        parse_stack.pop_back();
        
        if (std::holds_alternative<std::string>(top)) {
            // If we expect a terminal on the stack
            std::string expected = std::get<std::string>(top);
            
            if (expected == "$") {
                // If we reached the EOF marker, check if input is also at EOF
                if (current_token->type == TokenType::Eof) {
                    return true; // Successful parse
                } else {
                    syntaxError("Expected end of file, got " + current_token->lexeme);
                    return false;
                }
            } else if (expected == EPSILON) {
                // Epsilon production, do nothing
                continue;
            } else if (expected == "{") {
                // Opening a new block scope
                if (current_token->lexeme == expected) {
                    if (verbose) {
                        std::cout << "Entering new scope at {" << std::endl;
                    }
                    symbol_table.enterScope();
                    tokens.advance();
                    current_token = &tokens.peek();
                } else {
                    syntaxError("Expected '{', got '" + current_token->lexeme + "'");
                    tokens.advance();
                    current_token = &tokens.peek();
                    return false;
                }
            } else if (expected == "}") {
                // Closing a block scope
                if (current_token->lexeme == expected) {
                    if (verbose) {
                        std::cout << "Exiting scope at }" << std::endl;
                    }
                    symbol_table.exitScope();
                    tokens.advance();
                    current_token = &tokens.peek();
                } else {
                    syntaxError("Expected '}', got '" + current_token->lexeme + "'");
                    tokens.advance();
                    current_token = &tokens.peek();
                    return false;
                }
            } else if (expected == "int" || expected == "float") {
                // Capture the type for declarations
                if (current_token->lexeme == expected) {
                    if (verbose) {
                        std::cout << "Type declaration: " << expected << std::endl;
                    }
                    current_type = (expected == "int") ? SymbolType::INT : SymbolType::FLOAT;
                    processing_declaration = true;
                    tokens.advance();
                    current_token = &tokens.peek();
                } else {
                    syntaxError("Expected '" + expected + "', got '" + current_token->lexeme + "'");
                    tokens.advance();
                    current_token = &tokens.peek();
                    return false;
                }
            } else if (expected == ";") {
                // End of declaration or statement
                if (current_token->lexeme == expected) {
                    if (processing_declaration && !current_identifier.empty()) {
                        // Finalize the declaration
                        if (verbose) {
                            std::cout << "Adding symbol to table: " << current_identifier << " of type " 
                                     << (current_type == SymbolType::INT ? "int" : "float") << std::endl;
                        }
                        
                        if (!symbol_table.insert(current_identifier, current_type)) {
                            // Report redeclaration error
                            error_reporter.error(current_token->loc, "Redeclaration of variable '%s'", 
                                               current_identifier.c_str());
                        }
                        
                        // Reset declaration tracking
                        current_identifier = "";
                        processing_declaration = false;
                    }
                    tokens.advance();
                    current_token = &tokens.peek();
                } else {
                    syntaxError("Expected ';', got '" + current_token->lexeme + "'");
                    tokens.advance();
                    current_token = &tokens.peek();
                    return false;
                }
            } else {
                // Match terminal with current input token
                if (current_token->lexeme == expected) {
                    if (verbose) {
                        std::cout << "Matched token: " << current_token->lexeme << std::endl;
                    }
                    tokens.advance();
                    current_token = &tokens.peek();
                } else {
                    syntaxError("Expected '" + expected + "', got '" + current_token->lexeme + "'");
                    // Skip the current token and try to recover
                    tokens.advance();
                    current_token = &tokens.peek();
                    return false;
                }
            }
        } else if (std::holds_alternative<TokenType>(top)) {
            // If we expect a token type on the stack
            TokenType expected = std::get<TokenType>(top);
            
            if (current_token->type == expected) {
                if (verbose) {
                    std::cout << "Matched token type: " << static_cast<int>(current_token->type) << std::endl;
                }
                
                // Capture identifiers for declarations and references
                if (expected == TokenType::Identifier) {
                    if (processing_declaration) {
                        // Store the identifier name for the declaration
                        current_identifier = current_token->lexeme;
                        if (verbose) {
                            std::cout << "Captured identifier for declaration: " << current_identifier << std::endl;
                        }
                    } else {
                        // For variable references, check if the variable is declared
                        SymbolInfo* info = symbol_table.lookup(current_token->lexeme);
                        if (!info) {
                            error_reporter.error(current_token->loc, "Use of undeclared variable '%s'", 
                                               current_token->lexeme.c_str());
                        }
                    }
                }
                
                tokens.advance();
                current_token = &tokens.peek();
            } else {
                std::string expectedStr;
                switch (expected) {
                    case TokenType::Identifier: expectedStr = "identifier"; break;
                    case TokenType::IntegerLiteral: expectedStr = "integer literal"; break;
                    case TokenType::FloatLiteral: expectedStr = "float literal"; break;
                    default: expectedStr = "unknown token type";
                }
                
                syntaxError("Expected " + expectedStr + ", got '" + current_token->lexeme + "'");
                // Skip the current token and try to recover
                tokens.advance();
                current_token = &tokens.peek();
                return false;
            }
        } else {
            // Non-terminal on top of stack, look up in parse table
            NonTerminal nonterm = std::get<NonTerminal>(top);
            
            // For verbose mode, print what we're processing
            if (verbose) {
                std::cout << "Processing non-terminal: " << nonTerminalToString(nonterm) << std::endl;
            }
            
            // Special case for DECLARATION - prepare to process a new declaration
            if (nonterm == NonTerminal::DECLARATION) {
                processing_declaration = true;
                current_type = SymbolType::UNKNOWN;
                current_identifier = "";
            }
            
            // Special case for STATEMENT_LIST to avoid infinite loops with epsilon productions
            if (nonterm == NonTerminal::STATEMENT_LIST) {
                // If we see '}', we use the epsilon production
                if (current_token->type == TokenType::Punctuation && current_token->lexeme == "}") {
                    if (verbose) {
                        std::cout << "} found, using epsilon for STATEMENT_LIST" << std::endl;
                    }
                    continue; // Skip to next iteration
                }
                
                // Check if we are at the beginning of a statement
                bool isStatementStart = false;
                
                // Check for statement start tokens
                if (current_token->type == TokenType::Keyword) {
                    // Keywords that can start a statement: int, float, while, return
                    if (current_token->lexeme == "int" || current_token->lexeme == "float" || 
                        current_token->lexeme == "while" || current_token->lexeme == "return") {
                        isStatementStart = true;
                    }
                } else if (current_token->type == TokenType::Identifier) {
                    // Statements can start with identifiers (assignments or expressions)
                    isStatementStart = true;
                } else if (current_token->type == TokenType::IntegerLiteral || 
                          current_token->type == TokenType::FloatLiteral ||
                          (current_token->type == TokenType::Punctuation && 
                           (current_token->lexeme == "(" || current_token->lexeme == "++" || current_token->lexeme == "--"))) {
                    // Expressions can also start with literals or certain punctuation
                    isStatementStart = true;
                }
                
                if (isStatementStart) {
                    if (verbose) {
                        std::cout << "Statement found, using STATEMENT STATEMENT_LIST for STATEMENT_LIST" << std::endl;
                    }
                    // Push STATEMENT_LIST first (top of stack gets processed first)
                    parse_stack.push_back(NonTerminal::STATEMENT_LIST);
                    parse_stack.push_back(NonTerminal::STATEMENT);
                    continue; // Skip to next iteration
                } else {
                    // If not at start of statement, use epsilon production
                    if (verbose) {
                        std::cout << "No statement start found, using epsilon for STATEMENT_LIST" << std::endl;
                    }
                    continue; // Skip to next iteration
                }
            }
            
            // Special case for STATEMENT to help with decision making
            if (nonterm == NonTerminal::STATEMENT) {
                // Declarations start with type keywords
                if (current_token->type == TokenType::Keyword && 
                    (current_token->lexeme == "int" || current_token->lexeme == "float")) {
                    if (verbose) {
                        std::cout << "Type found, using DECLARATION for STATEMENT" << std::endl;
                    }
                    parse_stack.push_back(NonTerminal::DECLARATION);
                    continue;
                }
                
                // Assignments start with identifiers
                else if (current_token->type == TokenType::Identifier) {
                    // Look ahead to check if this is an assignment or just an expression
                    // We need to check if the next token is '='
                    Token* next_token = nullptr;
                    if (!tokens.isAtEnd()) {
                        tokens.advance();
                        next_token = &tokens.peek();
                        tokens.rewind(); // Go back to current token
                    }
                    
                    if (next_token && next_token->type == TokenType::Operator && 
                        next_token->lexeme == "=") {
                        if (verbose) {
                            std::cout << "Assignment found, using ASSIGNMENT for STATEMENT" << std::endl;
                        }
                        parse_stack.push_back(NonTerminal::ASSIGNMENT);
                    } else {
                        // This is an expression statement
                        if (verbose) {
                            std::cout << "Expression statement found, using EXPRESSION ; for STATEMENT" << std::endl;
                        }
                        parse_stack.push_back(";");
                        parse_stack.push_back(NonTerminal::EXPRESSION);
                    }
                    continue;
                }
                
                // Loops start with 'while'
                else if (current_token->type == TokenType::Keyword && current_token->lexeme == "while") {
                    if (verbose) {
                        std::cout << "While found, using LOOP for STATEMENT" << std::endl;
                    }
                    parse_stack.push_back(NonTerminal::LOOP);
                    continue;
                }
                
                // Return statements start with 'return'
                else if (current_token->type == TokenType::Keyword && current_token->lexeme == "return") {
                    if (verbose) {
                        std::cout << "Return found, using RETURN_STMT for STATEMENT" << std::endl;
                    }
                    parse_stack.push_back(NonTerminal::RETURN_STMT);
                    continue;
                }
                
                // Expression statements
                else if (current_token->type == TokenType::IntegerLiteral || 
                        current_token->type == TokenType::FloatLiteral ||
                        (current_token->type == TokenType::Punctuation && 
                        (current_token->lexeme == "(" || current_token->lexeme == "++" || current_token->lexeme == "--"))) {
                    if (verbose) {
                        std::cout << "Expression statement found, using EXPRESSION ; for STATEMENT" << std::endl;
                    }
                    parse_stack.push_back(";");
                    parse_stack.push_back(NonTerminal::EXPRESSION);
                    continue;
                }
                
                // If nothing matches, use epsilon
                else {
                    if (verbose) {
                        std::cout << "No matching statement type, using epsilon for STATEMENT" << std::endl;
                    }
                    continue;
                }
            }
            
            // Use current token to determine the input symbol for parse table lookup
            std::string input_symbol = current_token->lexeme;
            
            // Convert token type to string placeholder for lookup if needed
            if (current_token->type == TokenType::Identifier ||
                current_token->type == TokenType::IntegerLiteral ||
                current_token->type == TokenType::FloatLiteral) {
                std::string tokenTypeStr = "$" + std::to_string(static_cast<int>(current_token->type));
                input_symbol = tokenTypeStr;
            }
            
            // Look up production in parse table
            auto it = parse_table[nonterm].find(input_symbol);
            
            if (it != parse_table[nonterm].end() && it->second.production_index != NO_PRODUCTION) {
                // Valid production, push RHS onto stack in reverse order
                const Production* prod = it->second.production;
                
                if (verbose) {
                    std::cout << "Using production: " << nonTerminalToString(prod->lhs) << " →";
                    for (const auto& symbol : prod->rhs) {
                        if (std::holds_alternative<std::string>(symbol)) {
                            std::cout << " " << std::get<std::string>(symbol);
                        } else if (std::holds_alternative<TokenType>(symbol)) {
                            TokenType tokenType = std::get<TokenType>(symbol);
                            std::cout << " [TokenType:" << static_cast<int>(tokenType) << "]";
                        } else {
                            std::cout << " " << nonTerminalToString(std::get<NonTerminal>(symbol));
                        }
                    }
                    std::cout << std::endl;
                }
                
                // Skip epsilon - it doesn't need to be pushed onto the stack
                if (!(prod->rhs.size() == 1 && 
                    std::holds_alternative<std::string>(prod->rhs[0]) && 
                    std::get<std::string>(prod->rhs[0]) == EPSILON)) {
                    
                    // Push symbols in reverse order
                    for (auto it = prod->rhs.rbegin(); it != prod->rhs.rend(); ++it) {
                        parse_stack.push_back(*it);
                    }
                }
            } else {
                // Error: No valid production for the current input
                reportParseError(nonterm);
                
                // Skip current token as a basic error recovery strategy
                tokens.advance();
                current_token = &tokens.peek();
                return false;
            }
        }
    }
    
    // Check if we hit the iteration limit
    if (iterations >= max_iterations) {
        error_reporter.error(current_token->loc, "Parsing aborted due to too many iterations (possible infinite loop)");
        return false;
    }
    
    // If we exhausted the parse stack but not the input, we have an error
    if (current_token->type != TokenType::Eof) {
        syntaxError("Unexpected token: " + current_token->lexeme);
        return false;
    }
    
    return true;
}

void Parser::reportParseError(NonTerminal nonterm) {
    std::stringstream error_msg;
    error_msg << "Unexpected token '" << current_token->lexeme << "' of type '" 
              << tokenTypeToString(current_token->type) << "' for non-terminal '" 
              << nonTerminalToString(nonterm) << "'";
    
    error_msg << "\nExpected one of: ";
    bool first = true;
    
    // List expected tokens based on the parse table
    for (const auto& entry : parse_table[nonterm]) {
        if (entry.second.production_index != NO_PRODUCTION) {
            if (!first) {
                error_msg << ", ";
            }
            error_msg << "'" << entry.first << "'";
            first = false;
        }
    }
    
    error_reporter.error(current_token->loc, "%s", error_msg.str().c_str());
}

void Parser::syntaxError(const std::string& message) {
    error_reporter.error(current_token->loc, "%s", message.c_str());
}

// Add the missing Parser methods
const FirstFollowSets& Parser::getFirstFollowSets() const {
    return first_follow;
}

void Parser::printParseTable() const {
    // Collect all terminals used in the parse table for columns
    std::set<std::string> all_terminals;
    for (int i = 0; i <= static_cast<int>(NonTerminal::FACTOR_TAIL); i++) {
        NonTerminal nt = static_cast<NonTerminal>(i);
        for (const auto& entry : parse_table.at(nt)) {
            if (entry.second.production_index != NO_PRODUCTION) {
                all_terminals.insert(entry.first);
            }
        }
    }
    
    // Print table header with terminals
    std::cout << "\nLL(1) PARSE TABLE:\n";
    std::cout << "------------------------------------------------------\n";
    std::cout << "NON-TERMINAL | ";
    for (const auto& terminal : all_terminals) {
        std::cout << terminal << " | ";
    }
    std::cout << "\n------------------------------------------------------\n";
    
    // Print each row (non-terminal)
    for (int i = 0; i <= static_cast<int>(NonTerminal::FACTOR_TAIL); i++) {
        NonTerminal nt = static_cast<NonTerminal>(i);
        std::cout << std::left << std::setw(13) << nonTerminalToString(nt) << "| ";
        
        // For each terminal, print the corresponding production
        for (const auto& terminal : all_terminals) {
            auto it = parse_table.at(nt).find(terminal);
            if (it != parse_table.at(nt).end() && it->second.production_index != NO_PRODUCTION) {
                // Print a short representation of the production
                std::cout << it->second.production_index << " | ";
            } else {
                std::cout << "  | "; // Empty cell
            }
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------------------------------\n";
    
    // Print production legends
    std::cout << "\nProduction Legend:\n";
    for (size_t i = 0; i < first_follow.grammar.size(); i++) {
        const Production& prod = first_follow.grammar[i];
        std::cout << i << ": " << nonTerminalToString(prod.lhs) << " → ";
        
        for (const auto& symbol : prod.rhs) {
            if (std::holds_alternative<std::string>(symbol)) {
                std::cout << std::get<std::string>(symbol) << " ";
            } else if (std::holds_alternative<TokenType>(symbol)) {
                TokenType tokenType = std::get<TokenType>(symbol);
                switch(tokenType) {
                    case TokenType::Identifier:
                        std::cout << "IDENTIFIER ";
                        break;
                    case TokenType::IntegerLiteral:
                        std::cout << "INTEGER_LITERAL ";
                        break;
                    case TokenType::FloatLiteral:
                        std::cout << "FLOAT_LITERAL ";
                        break;
                    default:
                        std::cout << "TOKEN_TYPE(" << static_cast<int>(tokenType) << ") ";
                }
            } else {
                std::cout << nonTerminalToString(std::get<NonTerminal>(symbol)) << " ";
            }
        }
        std::cout << std::endl;
    }
}

void Parser::setVerbose(bool enable) {
    verbose = enable;
}

// Add getTokenTypeString implementation for FirstFollowSets
std::string FirstFollowSets::getTokenTypeString(TokenType type) const {
    switch(type) {
        case TokenType::Identifier:
            return "IDENTIFIER";
        case TokenType::IntegerLiteral:
            return "INTEGER_LITERAL";
        case TokenType::FloatLiteral:
            return "FLOAT_LITERAL";
        case TokenType::Keyword:
            return "KEYWORD";
        case TokenType::Operator:
            return "OPERATOR";
        case TokenType::Punctuation:
            return "PUNCTUATION";
        case TokenType::StringLiteral:
            return "STRING_LITERAL";
        case TokenType::Eof:
            return "EOF";
        case TokenType::Error:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

// Implement additional Parser methods
std::string Parser::getTokenKey(const Token& token) {
    if (token.type == TokenType::Identifier ||
        token.type == TokenType::IntegerLiteral ||
        token.type == TokenType::FloatLiteral) {
        return "$" + std::to_string(static_cast<int>(token.type));
    }
    return token.lexeme;
}

bool Parser::matchToken(const std::variant<std::string, TokenType>& expected) {
    if (std::holds_alternative<std::string>(expected)) {
        std::string lexeme = std::get<std::string>(expected);
        if (current_token->lexeme == lexeme) {
            tokens.advance();
            current_token = &tokens.peek();
            return true;
        }
        return false;
    } else if (std::holds_alternative<TokenType>(expected)) {
        TokenType type = std::get<TokenType>(expected);
        if (current_token->type == type) {
            tokens.advance();
            current_token = &tokens.peek();
            return true;
        }
        return false;
    }
    return false;
}

void Parser::initGrammar() {
    // Initialize grammar productions
    productions.clear();
    
    // Program structure
    productions.emplace_back(NonTerminal::PROGRAM, std::vector<std::variant<NonTerminal, std::string, TokenType>>{NonTerminal::MAIN_FUNCTION});
    
    // Main function
    productions.emplace_back(NonTerminal::MAIN_FUNCTION, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        "int", "main", "(", ")", "{", NonTerminal::STATEMENT_LIST, "}"
    });
    
    // Statement list
    productions.emplace_back(NonTerminal::STATEMENT_LIST, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        NonTerminal::STATEMENT, NonTerminal::STATEMENT_LIST
    });
    productions.emplace_back(NonTerminal::STATEMENT_LIST, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        EPSILON // Empty statement list
    });
    
    // Statement types
    productions.emplace_back(NonTerminal::STATEMENT, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        NonTerminal::DECLARATION // Variable declaration
    });
    productions.emplace_back(NonTerminal::STATEMENT, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        NonTerminal::ASSIGNMENT // Assignment statement
    });
    productions.emplace_back(NonTerminal::STATEMENT, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        NonTerminal::LOOP // Loop statement
    });
    productions.emplace_back(NonTerminal::STATEMENT, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        NonTerminal::RETURN_STMT // Return statement
    });
    productions.emplace_back(NonTerminal::STATEMENT, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        NonTerminal::EXPRESSION, ";" // Expression statement (e.g., i++;)
    });
    productions.emplace_back(NonTerminal::STATEMENT, std::vector<std::variant<NonTerminal, std::string, TokenType>>{
        EPSILON // Empty statement
    });
    
    // Declaration
    // ... existing code ...
}
