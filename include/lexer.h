#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "error.h"
#include <string>
#include <unordered_map>

class Lexer {
private:
    FILE* source;
    char* buffer;
    size_t buffer_size;
    size_t line;
    size_t column;
    size_t pos;  // Track position in buffer
    char current_char;

    static const std::unordered_map<std::string, KeywordType> keywords;

public:
    Lexer(std::string filename);
    ~Lexer();
    TokenStream tokenize();

private:
    void advance();
    void skipWhitespace();
    Token identifier();
    Token number();
};

#endif 