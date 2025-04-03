#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

// Enumeration for the possible types in our language
enum class SymbolType {
    INT,
    FLOAT,
    VOID,
    UNKNOWN
};

// Structure to hold information about a symbol
struct SymbolInfo {
    std::string name;
    SymbolType type;
    int scope_level;
    bool is_initialized;
    
    // Value, if known at compile time
    union {
        int int_value;
        float float_value;
    } value;
    
    SymbolInfo() : name(""), type(SymbolType::UNKNOWN), scope_level(0), is_initialized(false) {}
    
    SymbolInfo(const std::string& name, SymbolType type, int scope) 
        : name(name), type(type), scope_level(scope), is_initialized(false) {}
};

// Class that manages symbols and their scopes
class SymbolTable {
private:
    // Vector of symbol tables, one for each scope level
    std::vector<std::unordered_map<std::string, SymbolInfo>> scopes;
    
    // Current scope level
    int current_scope;
    
public:
    SymbolTable();
    
    // Enter a new scope
    void enterScope();
    
    // Exit the current scope
    void exitScope();
    
    // Add a new symbol to the current scope
    bool insert(const std::string& name, SymbolType type);
    
    // Update a symbol's initialization status
    bool setInitialized(const std::string& name);
    
    // Update a symbol's value (if constant)
    bool setValue(const std::string& name, int value);
    bool setValue(const std::string& name, float value);
    
    // Look up a symbol in all visible scopes
    SymbolInfo* lookup(const std::string& name);
    
    // Check if a symbol is already defined in the current scope
    bool isDefined(const std::string& name);
    
    // For testing and debugging
    void printTable() const;
    
    // Get the current scope level
    int getCurrentScope() const;
    
    // Get the number of symbols in the current scope
    int getCurrentScopeSize() const;
    
    // Get all symbols in the symbol table
    std::vector<SymbolInfo> getAllSymbols() const;
};

#endif // SYMBOL_TABLE_H
