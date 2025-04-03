#ifndef TOKEN_H
#define TOKEN_H

#include <error.h>
#include <string>
#include <vector>

enum class TokenType {
    Keyword,
    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    Operator,
    Punctuation,
    Eof,
    Error
};

enum class KeywordType {
    Auto, Const, Double, Float, Int, Struct, Break, Continue, Else, If, For,
    Short, Unsigned, Long, Signed, Switch, Case, Default, Void, Enum, Goto,
    Register, Sizeof, Typedef, Volatile, Char, Do, Extern, Static, Union, While, Return
};

enum class OperatorType {
    ARROW, INC, DEC, SHL, SHR, LE, GE, EQ, NE, AND, OR, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, ADD_ASSIGN, SUB_ASSIGN, SHL_ASSIGN, SHR_ASSIGN, AND_ASSIGN, XOR_ASSIGN, OR_ASSIGN, PLUS, MINUS, STAR, SLASH, PERCENT, LESS, GREATER, EQUAL, DOT, COMMA, SEMICOLON, COLON, BANG, QUESTION, TILDE, AMPERSAND, PIPE, CARET
};

enum class PunctuationType {
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, LANGLE, RANGLE
};

enum class LiteralType {
    Integer,
    Float,
    String,
    Character,
    Boolean,
};

typedef union{
    int int_value;
    char char_value;
    float float_value;
    char* string_value;
    double double_value;
} TokenValue;

struct Token {
    TokenType type;
    union {
        KeywordType keyword;
        OperatorType op;
        LiteralType literal;
        PunctuationType punct;
    } subtype;

    SourceLocation loc;
    TokenValue value;
    std::string lexeme;
};

class TokenStream {
private:
    std::vector<Token> tokens;
    size_t current;
public:
    TokenStream() : current(0) {}
    TokenStream(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}
    Token& peek();
    void add(Token token);
    Token& advance();
    bool isAtEnd() const;
    void reset();
    void synchronize();
};

#endif // TOKEN_H
