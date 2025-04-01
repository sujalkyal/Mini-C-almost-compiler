#include "token.h"

// TokenStream implementation
Token& TokenStream::peek() {
    if (isAtEnd()) {
        static Token eofToken;
        eofToken.type = TokenType::Eof;
        return eofToken;
    }
    return tokens[current];
}

void TokenStream::add(Token token) {
    tokens.push_back(token);
}

Token& TokenStream::advance() {
    if (!isAtEnd()) {
        Token& currentToken = tokens[current];
        current++;
        return currentToken;
    }
    return tokens[current];
}

bool TokenStream::isAtEnd() const {
    return current >= tokens.size();
}

void TokenStream::reset() {
    current = 0;
}

void TokenStream::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (peek().type == TokenType::Operator && 
            peek().subtype.op == OperatorType::SEMICOLON) {
            return;
        }
        
        if (peek().type == TokenType::Keyword) {
            KeywordType kw = peek().subtype.keyword;
            if (kw == KeywordType::Int || kw == KeywordType::Float ||
                kw == KeywordType::While || kw == KeywordType::If ||
                kw == KeywordType::Return) {
                return;
            }
        }
        
        advance();
    }
}
