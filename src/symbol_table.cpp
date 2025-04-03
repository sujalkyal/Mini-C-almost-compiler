#include "symbol_table.h"
#include <iostream>

SymbolTable::SymbolTable() : current_scope(0) {
    // Initialize with global scope
    scopes.push_back(std::unordered_map<std::string, SymbolInfo>());
}

void SymbolTable::enterScope() {
    // Create a new scope
    current_scope++;
    if (current_scope >= scopes.size()) {
        scopes.push_back(std::unordered_map<std::string, SymbolInfo>());
    }
}

void SymbolTable::exitScope() {
    // Exit current scope
    if (current_scope > 0) {
        current_scope--;
    }
}

bool SymbolTable::insert(const std::string& name, SymbolType type) {
    // Check if the symbol is already defined in the current scope
    if (isDefined(name)) {
        return false;
    }
    
    // Add the symbol to the current scope
    SymbolInfo info(name, type, current_scope);
    scopes[current_scope][name] = info;
    return true;
}

bool SymbolTable::setInitialized(const std::string& name) {
    // Find the symbol
    SymbolInfo* info = lookup(name);
    if (!info) {
        return false;
    }
    
    // Update initialization status
    info->is_initialized = true;
    return true;
}

bool SymbolTable::setValue(const std::string& name, int value) {
    // Find the symbol
    SymbolInfo* info = lookup(name);
    if (!info || info->type != SymbolType::INT) {
        return false;
    }
    
    // Update value
    info->value.int_value = value;
    info->is_initialized = true;
    return true;
}

bool SymbolTable::setValue(const std::string& name, float value) {
    // Find the symbol
    SymbolInfo* info = lookup(name);
    if (!info || info->type != SymbolType::FLOAT) {
        return false;
    }
    
    // Update value
    info->value.float_value = value;
    info->is_initialized = true;
    return true;
}

SymbolInfo* SymbolTable::lookup(const std::string& name) {
    // Check each scope from current to global
    for (int i = current_scope; i >= 0; i--) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            return &(it->second);
        }
    }
    
    // Symbol not found
    return nullptr;
}

bool SymbolTable::isDefined(const std::string& name) {
    // Check only the current scope
    return scopes[current_scope].find(name) != scopes[current_scope].end();
}

void SymbolTable::printTable() const {
    std::cout << "Symbol Table Contents:" << std::endl;
    std::cout << "=======================" << std::endl;
    
    for (size_t i = 0; i < scopes.size(); i++) {
        std::cout << "Scope Level " << i << ":" << std::endl;
        
        for (const auto& entry : scopes[i]) {
            const SymbolInfo& info = entry.second;
            std::cout << "  " << info.name << " (";
            
            switch (info.type) {
                case SymbolType::INT:
                    std::cout << "int";
                    break;
                case SymbolType::FLOAT:
                    std::cout << "float";
                    break;
                case SymbolType::VOID:
                    std::cout << "void";
                    break;
                default:
                    std::cout << "unknown";
            }
            
            std::cout << ") - ";
            std::cout << (info.is_initialized ? "initialized" : "uninitialized");
            
            if (info.is_initialized) {
                if (info.type == SymbolType::INT) {
                    std::cout << " = " << info.value.int_value;
                } else if (info.type == SymbolType::FLOAT) {
                    std::cout << " = " << info.value.float_value;
                }
            }
            
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
    }
}

int SymbolTable::getCurrentScope() const {
    return current_scope;
}

int SymbolTable::getCurrentScopeSize() const {
    return scopes[current_scope].size();
}

std::vector<SymbolInfo> SymbolTable::getAllSymbols() const {
    std::vector<SymbolInfo> result;
    
    for (const auto& scope : scopes) {
        for (const auto& entry : scope) {
            result.push_back(entry.second);
        }
    }
    
    return result;
}
