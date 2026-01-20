/*
 * Reverse Polish Notation (RPN) Calculator
 * Example: '10 5 /' is 10 / 5 = 2
 * Example: '3 4 2 * +' is 3 + (4 * 2) = 11
 * Example: '3.14 180 / sin' is sin(3.14/180) (approx)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // For sin, cos, etc.

#define STACK_SIZE 100

// --- Simple Stack Implementation ---
double stack[STACK_SIZE];
int top = -1; // Stack is empty

void push(double value) {
    if (top >= STACK_SIZE - 1) {
        printf("Error: Stack overflow\n");
    } else {
        stack[++top] = value;
    }
}

double pop() {
    if (top < 0) {
        printf("Error: Stack underflow\n");
        return 0; // Or exit
    } else {
        return stack[top--];
    }
}
// ---------------------------------

int main() {
    char input[256];
    
    printf("RPN Calculator (type 'q' to quit)\n");
    
    while (1) {
        printf(">");
        if (!fgets(input, 256, stdin)) {
            break; // EOF
        }
        
        top = -1; // Clear stack for new expression
        
        char *token = strtok(input, " \n");
        
        while (token != NULL) {
            // Check for quit
            if (strcmp(token, "q") == 0) {
                exit(0);
            }
            
            // Try to convert to a number
            double num;
            if (sscanf(token, "%lf", &num) == 1) {
                push(num);
            } 
            // Check for operators
            else if (strcmp(token, "+") == 0) {
                push(pop() + pop());
            } else if (strcmp(token, "-") == 0) {
                double b = pop();
                double a = pop();
                push(a - b);
            } else if (strcmp(token, "*") == 0) {
                push(pop() * pop());
            } else if (strcmp(token, "/") == 0) {
                double b = pop();
                double a = pop();
                if (b == 0) {
                    printf("Error: Division by zero\n");
                } else {
                    push(a / b);
                }
            }
            // Check for math functions
            else if (strcmp(token, "sin") == 0) {
                push(sin(pop()));
            } else if (strcmp(token, "cos") == 0) {
                push(cos(pop()));
            } else if (strcmp(token, "log") == 0) {
                push(log10(pop()));
            } else if (strcmp(token, "pow") == 0) {
                double b = pop(); // Exponent
                double a = pop(); // Base
                push(pow(a, b));
            } else {
                printf("Error: Unknown token '%s'\n", token);
            }
            
            token = strtok(NULL, " \n");
        }
        
        // Print the final result (should be only item on stack)
        if (top == 0) {
            printf("Result: %f\n", stack[top]);
        } else if(top > 0) {
            printf("Error: Too many items on stack\n");
        }
    }
    return 0;
}
