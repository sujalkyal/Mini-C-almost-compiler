// Sample program for testing the mini-C compiler
// This program uses basic language features:
// - main function
// - variable declarations with assignments
// - while loop with relational operators
// - arithmetic expressions
// - increment operations

main() {
    // Variable declarations with initializations
    int count = 0;
    int sum = 0;
    float average = 0.0;
    int max = 10;
    
    // While loop with a simple condition
    while (count < max) {
        // Arithmetic expression
        sum = sum + count;
        
        // Increment operation for loop variable
        count++;
        
        // Declaration inside loop
        int square = count * count;
    }
    
    // Compute average using division
    average = sum / max;
} 