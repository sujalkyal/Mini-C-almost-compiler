#include "lexer.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <iostream>
#include <unordered_map>

const std::unordered_map<std::string, KeywordType> Lexer::keywords = {
    {"auto", KeywordType::Auto},
    {"const", KeywordType::Const},
    {"double", KeywordType::Double},
    {"float", KeywordType::Float},
    {"int", KeywordType::Int},
    {"struct", KeywordType::Struct},
    {"break", KeywordType::Break},
    {"continue", KeywordType::Continue},
    {"else", KeywordType::Else},
    {"if", KeywordType::If},
    {"for", KeywordType::For},
    {"short", KeywordType::Short},
    {"unsigned", KeywordType::Unsigned},
    {"long", KeywordType::Long},
    {"signed", KeywordType::Signed},
    {"switch", KeywordType::Switch},
    {"case", KeywordType::Case},
    {"default", KeywordType::Default},
    {"void", KeywordType::Void},
    {"enum", KeywordType::Enum},
    {"goto", KeywordType::Goto},
    {"register", KeywordType::Register},
    {"sizeof", KeywordType::Sizeof},
    {"typedef", KeywordType::Typedef},
    {"volatile", KeywordType::Volatile},
    {"char", KeywordType::Char},
    {"do", KeywordType::Do},
    {"extern", KeywordType::Extern},
    {"static", KeywordType::Static},
    {"union", KeywordType::Union},
    {"while", KeywordType::While},
    {"return", KeywordType::Return}
};

Lexer::Lexer(std::string filename) {
    source = fopen(filename.c_str(), "r");
    if (!source) {
        SourceLocation loc(filename, 0, 0);
        errorReporter.error(loc, "Cannot open source file '%s'", filename.c_str());
        buffer = nullptr;
        buffer_size = 0;
        return;
    }
    
    fseek(source, 0, SEEK_END);
    buffer_size = ftell(source);
    fseek(source, 0, SEEK_SET);
    
    // Allocate buffer and read file
    buffer = new char[buffer_size + 1];
    if (buffer) {
        size_t bytes_read = fread(buffer, 1, buffer_size, source);
        buffer[bytes_read] = '\0';  // Null terminate
        
        if (bytes_read < buffer_size) {
            SourceLocation loc(filename, 0, 0);
            errorReporter.warning(loc, "File read incomplete: expected %zu bytes, got %zu", 
                                buffer_size, bytes_read);
            buffer_size = bytes_read;
        }
    } else {
        SourceLocation loc(filename, 0, 0);
        errorReporter.error(loc, "Memory allocation failed for file '%s'", filename.c_str());
        buffer_size = 0;
    }
    
    line = 1;
    column = 1;
    
    // Initialize error reporter with filename
    errorReporter.init(filename);
    
    if (buffer && buffer_size > 0) {
        current_char = buffer[0];
    } else {
        current_char = '\0';
    }
}

Lexer::~Lexer() {
    if (source) {
        fclose(source);
    }
    
    if (buffer) {
        delete[] buffer;
    }
}

void Lexer::advance() {
    static size_t pos = 0;
    
    if (current_char == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    
    pos++;
    if (pos < buffer_size) {
        current_char = buffer[pos];
    } else {
        current_char = '\0';  // EOF
    }
}

void Lexer::skipWhitespace() {
    while (isspace(current_char)) {
        advance();
    }
}

Token Lexer::identifier() {
    Token token;
    std::string id;
    SourceLocation loc(errorReporter.getCurrentFile(), line, column);
    
    // Record starting location
    token.loc = loc;
    
    // Read identifier
    while (isalnum(current_char) || current_char == '_') {
        id += current_char;
        advance();
    }
    
    token.lexeme = id;
    
    // Check if it's a keyword
    auto it = keywords.find(id);
    if (it != keywords.end()) {
        token.type = TokenType::Keyword;
        token.subtype.keyword = it->second;
    } else {
        token.type = TokenType::Identifier;
    }
    
    return token;
}

Token Lexer::number() {
    Token token;
    std::string num_str;
    SourceLocation loc(errorReporter.getCurrentFile(), line, column);
    bool is_float = false;
    
    token.loc = loc;
    
    while (isdigit(current_char)) {
        num_str += current_char;
        advance();
    }
    
    // Check for decimal point
    if (current_char == '.') {
        is_float = true;
        num_str += current_char;
        advance();
        
        // Read fractional part
        while (isdigit(current_char)) {
            num_str += current_char;
            advance();
        }
    }
    
    token.lexeme = num_str;
    
    // Convert to numeric value
    if (is_float) {
        token.type = TokenType::FloatLiteral;
        token.value.float_value = std::stof(num_str);
    } else {
        token.type = TokenType::IntegerLiteral;
        token.value.int_value = std::stoi(num_str);
    }
    
    return token;
}

TokenStream Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (current_char != '\0') {
        skipWhitespace();
        
        if (current_char == '\0') {
            break;  // End of file
        }
        
        SourceLocation loc(errorReporter.getCurrentFile(), line, column);
        Token token;
        token.loc = loc;
        
        if (isalpha(current_char) || current_char == '_') {
            // Identifier or keyword
            tokens.push_back(identifier());
        }
        else if (isdigit(current_char)) {
            // Number
            tokens.push_back(number());
        }
        else if (current_char == '"') {
            // String literal
            advance();  // Skip opening quote
            std::string string_value;
            loc = SourceLocation(errorReporter.getCurrentFile(), line, column);
            
            while (current_char != '"' && current_char != '\0') {
                if (current_char == '\\') {
                    advance();  // Skip backslash
                    switch (current_char) {
                        case 'n': string_value += '\n'; break;
                        case 't': string_value += '\t'; break;
                        case 'r': string_value += '\r'; break;
                        case '\\': string_value += '\\'; break;
                        case '"': string_value += '"'; break;
                        default:
                            errorReporter.error(loc, "Invalid escape sequence '\\%c'", current_char);
                    }
                } else {
                    string_value += current_char;
                }
                advance();
            }
            
            if (current_char == '\0') {
                errorReporter.error(loc, "Unterminated string literal");
                token.type = TokenType::Error;
            } else {
                // Skip closing quote
                advance();
                
                token.type = TokenType::StringLiteral;
                token.subtype.literal = LiteralType::String;
                token.lexeme = "\"" + string_value + "\"";
                
                // Allocate memory for string value
                char* str_value = new char[string_value.length() + 1];
                strcpy(str_value, string_value.c_str());
                token.value.string_value = str_value;
            }
            tokens.push_back(token);
        }
        else {
            // Operators and punctuation
            token.loc = loc;
            
            switch (current_char) {
                case '(':
                    token.type = TokenType::Punctuation;
                    token.subtype.punct = PunctuationType::LPAREN;
                    token.lexeme = "(";
                    advance();
                    break;
                    
                case ')':
                    token.type = TokenType::Punctuation;
                    token.subtype.punct = PunctuationType::RPAREN;
                    token.lexeme = ")";
                    advance();
                    break;
                    
                case '{':
                    token.type = TokenType::Punctuation;
                    token.subtype.punct = PunctuationType::LBRACE;
                    token.lexeme = "{";
                    advance();
                    break;
                    
                case '}':
                    token.type = TokenType::Punctuation;
                    token.subtype.punct = PunctuationType::RBRACE;
                    token.lexeme = "}";
                    advance();
                    break;
                    
                case '[':
                    token.type = TokenType::Punctuation;
                    token.subtype.punct = PunctuationType::LBRACKET;
                    token.lexeme = "[";
                    advance();
                    break;
                    
                case ']':
                    token.type = TokenType::Punctuation;
                    token.subtype.punct = PunctuationType::RBRACKET;
                    token.lexeme = "]";
                    advance();
                    break;
                    
                case ';':
                    token.type = TokenType::Operator;
                    token.subtype.op = OperatorType::SEMICOLON;
                    token.lexeme = ";";
                    advance();
                    break;
                    
                case ',':
                    token.type = TokenType::Operator;
                    token.subtype.op = OperatorType::COMMA;
                    token.lexeme = ",";
                    advance();
                    break;
                    
                case '.':
                    token.type = TokenType::Operator;
                    token.subtype.op = OperatorType::DOT;
                    token.lexeme = ".";
                    advance();
                    break;
                    
                case '+':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '+') {
                        token.subtype.op = OperatorType::INC;
                        token.lexeme = "++";
                        advance();
                    } else if (current_char == '=') {
                        token.subtype.op = OperatorType::ADD_ASSIGN;
                        token.lexeme = "+=";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::PLUS;
                        token.lexeme = "+";
                    }
                    break;
                    
                case '-':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '-') {
                        token.subtype.op = OperatorType::DEC;
                        token.lexeme = "--";
                        advance();
                    } else if (current_char == '=') {
                        token.subtype.op = OperatorType::SUB_ASSIGN;
                        token.lexeme = "-=";
                        advance();
                    } else if (current_char == '>') {
                        token.subtype.op = OperatorType::ARROW;
                        token.lexeme = "->";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::MINUS;
                        token.lexeme = "-";
                    }
                    break;
                    
                case '*':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '=') {
                        token.subtype.op = OperatorType::MUL_ASSIGN;
                        token.lexeme = "*=";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::STAR;
                        token.lexeme = "*";
                    }
                    break;
                    
                case '/':
                    advance();
                    if (current_char == '/') {
                        // Single line comment
                        while (current_char != '\n' && current_char != '\0') {
                            advance();
                        }
                        continue;  // Skip creating token for comments
                    } else if (current_char == '*') {
                        // Multi-line comment
                        advance();
                        bool comment_ended = false;
                        while (!comment_ended && current_char != '\0') {
                            if (current_char == '*') {
                                advance();
                                if (current_char == '/') {
                                    comment_ended = true;
                                    advance();
                                }
                            } else {
                                advance();
                            }
                        }
                        
                        if (!comment_ended) {
                            errorReporter.error(loc, "Unterminated multi-line comment");
                        }
                        continue;  // Skip creating token for comments
                    } else if (current_char == '=') {
                        token.type = TokenType::Operator;
                        token.subtype.op = OperatorType::DIV_ASSIGN;
                        token.lexeme = "/=";
                        advance();
                    } else {
                        token.type = TokenType::Operator;
                        token.subtype.op = OperatorType::SLASH;
                        token.lexeme = "/";
                    }
                    break;
                    
                case '%':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '=') {
                        token.subtype.op = OperatorType::MOD_ASSIGN;
                        token.lexeme = "%=";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::PERCENT;
                        token.lexeme = "%";
                    }
                    break;
                    
                case '<':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '=') {
                        token.subtype.op = OperatorType::LE;
                        token.lexeme = "<=";
                        advance();
                    } else if (current_char == '<') {
                        advance();
                        if (current_char == '=') {
                            token.subtype.op = OperatorType::SHL_ASSIGN;
                            token.lexeme = "<<=";
                            advance();
                        } else {
                            token.subtype.op = OperatorType::SHL;
                            token.lexeme = "<<";
                        }
                    } else {
                        token.subtype.op = OperatorType::LESS;
                        token.lexeme = "<";
                    }
                    break;
                    
                case '>':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '=') {
                        token.subtype.op = OperatorType::GE;
                        token.lexeme = ">=";
                        advance();
                    } else if (current_char == '>') {
                        advance();
                        if (current_char == '=') {
                            token.subtype.op = OperatorType::SHR_ASSIGN;
                            token.lexeme = ">>=";
                            advance();
                        } else {
                            token.subtype.op = OperatorType::SHR;
                            token.lexeme = ">>";
                        }
                    } else {
                        token.subtype.op = OperatorType::GREATER;
                        token.lexeme = ">";
                    }
                    break;
                    
                case '=':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '=') {
                        token.subtype.op = OperatorType::EQ;
                        token.lexeme = "==";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::EQUAL;
                        token.lexeme = "=";
                    }
                    break;
                    
                case '!':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '=') {
                        token.subtype.op = OperatorType::NE;
                        token.lexeme = "!=";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::BANG;
                        token.lexeme = "!";
                    }
                    break;
                    
                case '&':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '&') {
                        token.subtype.op = OperatorType::AND;
                        token.lexeme = "&&";
                        advance();
                    } else if (current_char == '=') {
                        token.subtype.op = OperatorType::AND_ASSIGN;
                        token.lexeme = "&=";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::AMPERSAND;
                        token.lexeme = "&";
                    }
                    break;
                    
                case '|':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '|') {
                        token.subtype.op = OperatorType::OR;
                        token.lexeme = "||";
                        advance();
                    } else if (current_char == '=') {
                        token.subtype.op = OperatorType::OR_ASSIGN;
                        token.lexeme = "|=";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::PIPE;
                        token.lexeme = "|";
                    }
                    break;
                    
                case '^':
                    token.type = TokenType::Operator;
                    advance();
                    if (current_char == '=') {
                        token.subtype.op = OperatorType::XOR_ASSIGN;
                        token.lexeme = "^=";
                        advance();
                    } else {
                        token.subtype.op = OperatorType::CARET;
                        token.lexeme = "^";
                    }
                    break;
                    
                case '?':
                    token.type = TokenType::Operator;
                    token.subtype.op = OperatorType::QUESTION;
                    token.lexeme = "?";
                    advance();
                    break;
                    
                case ':':
                    token.type = TokenType::Operator;
                    token.subtype.op = OperatorType::COLON;
                    token.lexeme = ":";
                    advance();
                    break;
                    
                case '~':
                    token.type = TokenType::Operator;
                    token.subtype.op = OperatorType::TILDE;
                    token.lexeme = "~";
                    advance();
                    break;
                    
                default:
                    // Unrecognized character
                    errorReporter.error(loc, "Unexpected character '%c'", current_char);
                    token.type = TokenType::Error;
                    token.lexeme = current_char;
                    advance();
            }
            
            if (token.type != TokenType::Error) {
                tokens.push_back(token);
            }
        }
    }
    
    // Add EOF token
    Token eof_token;
    eof_token.type = TokenType::Eof;
    eof_token.loc = SourceLocation(errorReporter.getCurrentFile(), line, column);
    eof_token.lexeme = "<EOF>";
    tokens.push_back(eof_token);
    
    return TokenStream(tokens);
}
