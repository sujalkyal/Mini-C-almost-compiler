/* Complex test program with nested loops and conditionals */
int main() {
    int i = 0;
    int max = 5;
    float sum = 0.0;
    float avg = 0.0;

    /* Syntax errors for testing parser */
    int missing_semicolon = 10;
    
    /* Invalid expression */
    float invalid = 5 * 3;

    
    /* Missing operand */
    int incomplete = i + 3;
    
    /* Invalid variable declaration */
    int invalid = 5;
    
    /* Loop with nested conditional */
    while (i < max) {
        int j = 0;
        
        /* Nested loop */
        while (j < i) {
            sum = sum + j;
            j++;
        }
        
        i++;
    }
    
    /* Calculate average */
    // if (max > 0) {
    //     avg = sum / max;
    // }
    
    return 0;
} 