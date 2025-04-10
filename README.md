# Mini C-like Compiler

A simple compiler for a C-like language with limited features. This project implements a compiler frontend with lexical analysis, parsing, and symbol table management for a subset of the C programming language.

## Features

- **Lexical Analysis**: Tokenizes source code and identifies keywords, identifiers, literals, operators, and punctuation
- **Syntax Analysis**: Implements an LL(1) parser with FIRST and FOLLOW set computation
- **Symbol Table**: Tracks variables, their types, and scopes with proper nesting
- **Error Handling**: Provides detailed error messages with line and column information
- **Semantic Analysis**: Detects variable redeclarations and type mismatches
- **Command-line Interface**: Offers various debugging flags and options

## Language Specification

The language supports:
- **Data Types**: `int` and `float`
- **Control Structures**: `while` loops
- **Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`, `++`, `--`
  - Relational: `<`, `>`, `<=`, `>=`, `==`, `!=`
- **Program Structure**: Only a single `main()` function
- **Variables**: Declaration with optional initialization
- **Statements**: Assignment, expression, return

## Building the Project

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)

### Build Commands

```bash
# Clone the repository
git clone https://github.com/srinjoydutta03/Mini-C-almost-compiler.git
cd Mini-C-almost-compiler

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make
```

## Usage

The minicompiler provides several options for analyzing and debugging your C-like programs:

```bash
./minicompiler [options] <source_file>
```

### Command-line Options

- `--verbose`: Show detailed output from all compiler stages
- `--show-tokens`: Display token stream after lexical analysis
- `--show-parse-table`: Display the LL(1) parsing table
- `--show-first-follow`: Display FIRST and FOLLOW sets for the grammar
- `--show-symbol-table`: Display the final symbol table with all variables
- `--show-parse-steps`: Show detailed parsing steps during syntax analysis
- `--help`: Display help message

### Running the Tests

The project includes a comprehensive test suite for the lexer, parser, and symbol table:

```bash
# Run all tests
cd build
ctest

# Run specific test categories
./tests/lexer_tests
./tests/parser_tests
./tests/symbol_table_tests
```

### Example Usage

#### Basic Compilation

```bash
./minicompiler input.c
```

#### Debugging Lexical Analysis

```bash
./minicompiler --show-tokens input.c
```

#### Full Debugging Output

```bash
./minicompiler --verbose input.c
```

## Sample Programs

### Simple Program

```c
int main() {
    int i = 0;
    float x = 10.5;
    
    while (i < 10) {
        x = x + 1.5;
        i++;
    }
    
    return 0;
}
```

### Complex Program with Nested Blocks

```c
int main() {
    int i = 0;
    int max = 5;
    float sum = 0.0;
    
    while (i < max) {
        int j = 0;
        
        while (j < i) {
            sum = sum + j;
            j++;
        }
        
        i++;
    }
    
    return 0;
}
```

## Project Architecture

### Components

1. **Lexer** (`src/lexer.cpp`, `include/lexer.h`)
   - Tokenizes the source code
   - Handles lexical errors
   - Provides token statistics

2. **Parser** (`src/parser.cpp`, `include/parser.h`)
   - Implements an LL(1) parsing algorithm
   - Computes FIRST and FOLLOW sets
   - Builds and uses a parsing table
   - Handles syntax errors with recovery

3. **Symbol Table** (`src/symbol_table.cpp`, `include/symbol_table.h`)
   - Tracks variable declarations and their types
   - Manages nested scopes
   - Detects redeclaration errors

4. **Error Reporter** (`src/error.cpp`, `include/error.h`)
   - Provides formatted error messages
   - Tracks error counts and locations

### Compiler Phases

1. **Lexical Analysis**: Source code → Token stream
2. **Syntax Analysis**: Token stream → Parse tree
3. **Semantic Analysis**: Parse tree → Annotated AST with symbol information

## Understanding the Symbol Table

The symbol table uses a scope numbering scheme:

- **Scope Level 0**: Global variables (outside any function)
- **Scope Level 1**: Function parameters
- **Scope Level 2**: Variables in the main function
- **Scope Level 3+**: Variables in nested blocks (loops, conditionals)

Each symbol entry includes:
- Variable name
- Data type (int, float)
- Scope level
- Initialization status

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request




## Debugging Tips

- Use the `--verbose` flag for complete compiler output
- Check token stream with `--show-tokens` for lexical issues
- Examine the parse steps with `--show-parse-steps` for syntax problems
- Review the symbol table with `--show-symbol-table` for variable issues

## License

This project is licensed under the MIT License - see the LICENSE file for details.
