#include <iostream>
#include <fstream>
#include <cctype>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

// Token types
enum TokenType {
    KEYWORD, IDENTIFIER, NUMBER, OPERATOR, SYMBOL, UNKNOWN
};

// Keywords and Operators
const vector<string> keywords = {"int", "float", "while", "main"};
const vector<string> operators = {"+", "-", "*", "/", "++", "--", "=", "<", ">"};
const vector<char> symbols = {'(', ')', '{', '}', ';'};

// Symbol Table
unordered_map<string, string> symbolTable;

// Function to check if a word is a keyword
bool isKeyword(const string &word) {
    return find(keywords.begin(), keywords.end(), word) != keywords.end();
}

// Function to check if a word is an operator
bool isOperator(const string &word) {
    return find(operators.begin(), operators.end(), word) != operators.end();
}

// Function to check if a character is a symbol
bool isSymbol(char ch) {
    return find(symbols.begin(), symbols.end(), ch) != symbols.end();
}

// Function to check if a string is a valid number
bool isNumber(const string &word) {
    int dotCount = 0;
    for (char ch : word) {
        if (!isdigit(ch)) {
            if (ch == '.' && dotCount == 0) {
                dotCount++;
            } else {
                return false;
            }
        }
    }
    return true;
}

// Function to tokenize a given file
void tokenize(const string &filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    string token;
    char ch;
    vector<pair<string, TokenType>> tokens;

    while (file.get(ch)) {
        // Ignore whitespace
        if (isspace(ch)) {
            if (!token.empty()) {
                // Process token
                if (isKeyword(token)) {
                    tokens.push_back({token, KEYWORD});
                } else if (isNumber(token)) {
                    tokens.push_back({token, NUMBER});
                } else {
                    tokens.push_back({token, IDENTIFIER});
                    if (symbolTable.find(token) == symbolTable.end()) {
                        symbolTable[token] = "UNKNOWN";  // Type will be updated later
                    }
                }
                token.clear();
            }
        }
        // Handle operators
        else if (isOperator(string(1, ch))) {
            if (!token.empty()) {
                tokens.push_back({token, IDENTIFIER});
                token.clear();
            }

            // Handle two-character operators like ++, --
            char nextChar;
            file.get(nextChar);
            string op = string(1, ch) + string(1, nextChar);
            if (isOperator(op)) {
                tokens.push_back({op, OPERATOR});
            } else {
                tokens.push_back({string(1, ch), OPERATOR});
                file.unget();
            }
        }
        // Handle symbols
        else if (isSymbol(ch)) {
            if (!token.empty()) {
                tokens.push_back({token, IDENTIFIER});
                token.clear();
            }
            tokens.push_back({string(1, ch), SYMBOL});
        }
        // Handle identifiers and numbers
        else {
            token += ch;
        }
    }

    file.close();

    // Print tokens
    cout << "Tokens: " << endl;
    for (const auto &tok : tokens) {
        cout << tok.first << " -> ";
        switch (tok.second) {
            case KEYWORD: cout << "KEYWORD"; break;
            case IDENTIFIER: cout << "IDENTIFIER"; break;
            case NUMBER: cout << "NUMBER"; break;
            case OPERATOR: cout << "OPERATOR"; break;
            case SYMBOL: cout << "SYMBOL"; break;
            default: cout << "UNKNOWN";
        }
        cout << endl;
    }

    // Print Symbol Table
    cout << "\nSymbol Table: " << endl;
    for (const auto &entry : symbolTable) {
        cout << entry.first << " : " << entry.second << endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    tokenize(argv[1]);
    return 0;
}
