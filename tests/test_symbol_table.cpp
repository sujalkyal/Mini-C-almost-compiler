#include <iostream>
#include <cassert>
#include <unordered_set>
#include "symbol_table.h"

// This file will contain symbol table tests
// No main() function is needed here since test_lexer.cpp already has one

// Test basic symbol table functionality
void testBasicFunctionality() {
    std::cout << "Testing basic symbol table functionality..." << std::endl;
    
    SymbolTable table;
    
    // Test initial state
    assert(table.getCurrentScope() == 0);
    assert(table.getCurrentScopeSize() == 0);
    
    // Test inserting symbols
    bool result1 = table.insert("x", SymbolType::INT);
    assert(result1);
    assert(table.getCurrentScopeSize() == 1);
    
    bool result2 = table.insert("y", SymbolType::FLOAT);
    assert(result2);
    assert(table.getCurrentScopeSize() == 2);
    
    // Test looking up symbols
    SymbolInfo* info1 = table.lookup("x");
    assert(info1 != nullptr);
    assert(info1->name == "x");
    assert(info1->type == SymbolType::INT);
    assert(info1->scope_level == 0);
    assert(info1->is_initialized == false);
    
    SymbolInfo* info2 = table.lookup("y");
    assert(info2 != nullptr);
    assert(info2->name == "y");
    assert(info2->type == SymbolType::FLOAT);
    
    // Test looking up non-existent symbol
    SymbolInfo* info3 = table.lookup("z");
    assert(info3 == nullptr);
    
    std::cout << "Basic functionality test passed!" << std::endl;
}

// Test scoping rules
void testScoping() {
    std::cout << "Testing symbol table scoping..." << std::endl;
    
    SymbolTable table;
    
    // Insert symbols in global scope
    table.insert("x", SymbolType::INT);
    table.insert("y", SymbolType::FLOAT);
    
    // Enter a new scope
    table.enterScope();
    assert(table.getCurrentScope() == 1);
    assert(table.getCurrentScopeSize() == 0);
    
    // Insert in local scope
    table.insert("z", SymbolType::INT);
    assert(table.getCurrentScopeSize() == 1);
    
    // Test shadowing
    table.insert("x", SymbolType::FLOAT);
    
    // Lookup should find the local x, not the global one
    SymbolInfo* local_x = table.lookup("x");
    assert(local_x != nullptr);
    assert(local_x->type == SymbolType::FLOAT);
    assert(local_x->scope_level == 1);
    
    // We should still be able to see the global y
    SymbolInfo* global_y = table.lookup("y");
    assert(global_y != nullptr);
    assert(global_y->type == SymbolType::FLOAT);
    assert(global_y->scope_level == 0);
    
    // Exit back to global scope
    table.exitScope();
    assert(table.getCurrentScope() == 0);
    
    // Now x should be the global INT version
    SymbolInfo* global_x = table.lookup("x");
    assert(global_x != nullptr);
    assert(global_x->type == SymbolType::INT);
    assert(global_x->scope_level == 0);
    
    // z should no longer be visible
    SymbolInfo* z = table.lookup("z");
    assert(z == nullptr);
    
    std::cout << "Scoping test passed!" << std::endl;
}

// Test multiple nested scopes
void testNestedScopes() {
    std::cout << "Testing nested scopes..." << std::endl;
    
    SymbolTable table;
    
    // Global scope
    table.insert("a", SymbolType::INT);
    
    // Scope level 1
    table.enterScope();
    table.insert("b", SymbolType::INT);
    
    // Scope level 2
    table.enterScope();
    table.insert("c", SymbolType::INT);
    
    // Scope level 3
    table.enterScope();
    table.insert("d", SymbolType::INT);
    
    // We should see all variables
    assert(table.lookup("a") != nullptr);
    assert(table.lookup("b") != nullptr);
    assert(table.lookup("c") != nullptr);
    assert(table.lookup("d") != nullptr);
    
    // Exit to scope level 2
    table.exitScope();
    assert(table.lookup("a") != nullptr);
    assert(table.lookup("b") != nullptr);
    assert(table.lookup("c") != nullptr);
    assert(table.lookup("d") == nullptr);
    
    // Exit to scope level 1
    table.exitScope();
    assert(table.lookup("a") != nullptr);
    assert(table.lookup("b") != nullptr);
    assert(table.lookup("c") == nullptr);
    assert(table.lookup("d") == nullptr);
    
    // Exit to global scope
    table.exitScope();
    assert(table.lookup("a") != nullptr);
    assert(table.lookup("b") == nullptr);
    assert(table.lookup("c") == nullptr);
    assert(table.lookup("d") == nullptr);
    
    std::cout << "Nested scopes test passed!" << std::endl;
}

// Test symbol initialization and value handling
void testInitialization() {
    std::cout << "Testing symbol initialization..." << std::endl;
    
    SymbolTable table;
    
    // Insert symbols
    table.insert("int_var", SymbolType::INT);
    table.insert("float_var", SymbolType::FLOAT);
    
    // Check initial state
    SymbolInfo* int_info = table.lookup("int_var");
    assert(int_info != nullptr);
    assert(int_info->is_initialized == false);
    
    // Set values
    bool result1 = table.setValue("int_var", 42);
    assert(result1);
    
    bool result2 = table.setValue("float_var", 3.14f);
    assert(result2);
    
    // Check updated state
    assert(int_info->is_initialized == true);
    assert(int_info->value.int_value == 42);
    
    SymbolInfo* float_info = table.lookup("float_var");
    assert(float_info != nullptr);
    assert(float_info->is_initialized == true);
    assert(float_info->value.float_value == 3.14f);
    
    // Try setting wrong type
    bool result3 = table.setValue("int_var", 2.5f);
    assert(!result3);
    
    bool result4 = table.setValue("float_var", 10);
    assert(!result4);
    
    std::cout << "Initialization test passed!" << std::endl;
}

// Test duplicate symbol detection
void testDuplicateSymbols() {
    std::cout << "Testing duplicate symbol detection..." << std::endl;
    
    SymbolTable table;
    
    // Insert a symbol
    bool result1 = table.insert("x", SymbolType::INT);
    assert(result1);
    
    // Try to insert a duplicate in the same scope
    bool result2 = table.insert("x", SymbolType::FLOAT);
    assert(!result2);
    
    // Check that the original symbol is unchanged
    SymbolInfo* info = table.lookup("x");
    assert(info != nullptr);
    assert(info->type == SymbolType::INT);
    
    std::cout << "Duplicate symbol detection test passed!" << std::endl;
}

// Test retrieving all symbols
void testGetAllSymbols() {
    std::cout << "Testing getAllSymbols..." << std::endl;
    
    SymbolTable table;
    
    // Global scope
    table.insert("a", SymbolType::INT);
    table.insert("b", SymbolType::FLOAT);
    
    // Local scope
    table.enterScope();
    table.insert("c", SymbolType::INT);
    table.insert("d", SymbolType::FLOAT);
    
    // Get all symbols
    std::vector<SymbolInfo> symbols = table.getAllSymbols();
    
    // Check count
    assert(symbols.size() == 4);
    
    // Create a set of expected names
    std::unordered_set<std::string> expected_names = {"a", "b", "c", "d"};
    
    // Check each symbol exists
    for (const auto& symbol : symbols) {
        assert(expected_names.find(symbol.name) != expected_names.end());
        expected_names.erase(symbol.name);
    }
    
    // All names should have been found
    assert(expected_names.empty());
    
    std::cout << "getAllSymbols test passed!" << std::endl;
}

// Test symbol table with a while loop example
void testWhileLoopExample() {
    std::cout << "Testing symbol table with a while loop example..." << std::endl;
    
    SymbolTable table;
    
    // Global scope
    table.insert("i", SymbolType::INT);
    table.setValue("i", 0);
    
    table.insert("sum", SymbolType::INT);
    table.setValue("sum", 0);
    
    // Enter while loop scope
    table.enterScope();
    
    // Variables inside while loop
    table.insert("temp", SymbolType::INT);
    
    // Make sure we can see all variables
    assert(table.lookup("i") != nullptr);
    assert(table.lookup("sum") != nullptr);
    assert(table.lookup("temp") != nullptr);
    
    // Exit while loop scope
    table.exitScope();
    
    // Should not see temp anymore
    assert(table.lookup("i") != nullptr);
    assert(table.lookup("sum") != nullptr);
    assert(table.lookup("temp") == nullptr);
    
    std::cout << "While loop example test passed!" << std::endl;
}

// Main test runner function (replaces main to avoid duplicate symbols)
int run_symbol_table_tests() {
    std::cout << "==== RUNNING SYMBOL TABLE TESTS ====" << std::endl;
    
    // Run all test functions
    testBasicFunctionality();
    testScoping();
    testNestedScopes();
    testInitialization();
    testDuplicateSymbols();
    testGetAllSymbols();
    testWhileLoopExample();
    
    std::cout << "All symbol table tests passed!" << std::endl;
    
    return 0;
}