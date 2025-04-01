#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}Building Mini-C Compiler and Tests...${NC}"

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
echo -e "${BLUE}Configuring with CMake...${NC}"
cmake ..

# Build
echo -e "${BLUE}Building...${NC}"
make -j

# Check if build was successful
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"

# Ensure test files are available
echo -e "${BLUE}Setting up test files...${NC}"
mkdir -p test_files
cp -f ../tests/test_files/sample_program.c test_files/

# Run tests directly to get better debugging output
echo -e "${BLUE}Running tests directly...${NC}"

if [ "$1" == "lexer" ]; then
    ./tests/run_tests lexer
elif [ "$1" == "parser" ]; then
    ./tests/run_tests parser
elif [ "$1" == "symbol_table" ]; then
    ./tests/run_tests symbol_table
else
    ./tests/run_tests
fi

# Check if tests were successful
if [ $? -ne 0 ]; then
    echo -e "${RED}Tests failed!${NC}"
    exit 1
fi

echo -e "${GREEN}All tests passed!${NC}"
exit 0 