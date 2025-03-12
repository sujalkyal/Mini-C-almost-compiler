# Mini C-like Compiler

A simple compiler for a C-like language with limited features. This project includes a lexical analyzer, parser, and symbol table implementation.

## Features

- Lexical analysis of a C-like language
- Context-free grammar (CFG) for the language
- Top-down recursive descent parser
- Symbol table with type checking
- Error recovery mechanism
- Command-line tool with debugging options

## Language Specification

The language supports:
- Data types: `int` and `float`
- Variable declarations with initialization
- While loops with simple conditions
- Arithmetic operations: `+`, `-`, `*`, `/`, `++`, `--`
- Relational operators: `<`, `>`
- Only a `main()` function

## Building the Project

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler

### Build Commands

```bash
# Clone the repository
git clone https://github.com/yourusername/compiler.git
cd compiler

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Install (optional) - DON'T USE IT NOW
make install
```

## Usage

```bash
./minicompiler [options] <source_file>
```

### Options

- `--show-tokens`: Display all tokens after lexical analysis
- `--show-symbol-table`: Display the symbol table after parsing
- `--show-parse-steps`: Display detailed parsing steps
- `--help`: Display help message

### Example

```bash
./minicompiler --show-tokens --show-symbol-table input.c
```

## Development Guide

1. The lexical analyzer (`lexer.cpp`) scans the source code and produces tokens
2. The symbol table (`symbol_table.cpp`) stores variable information
3. The parser (`parser.cpp`) implements the top-down parsing algorithm
4. Error recovery is implemented in the parser

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.