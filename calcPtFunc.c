#include <stdio.h>
#include <stdlib.h>

typedef enum ops { ADD, SUB, MUL,
DIV, MOD, NOP } ops_t;
typedef int (*oper_pt)(int a, int b);
int calculate(ops_t i, int a, int b);
int add(int a, int b);
int sub(int a, int b);
int mul(int a, int b);
int dvd(int a, int b);
int mod(int a, int b);

int calculate (ops_t operation, int x, int y)
{
static const oper_pt ops[] = {
add, sub, mul, dvd, mod
};
return ops[operation] (x, y);
}
int add(int i, int j) {
return (i+j);
}
int sub(int i, int j) {
return (i-j);
}
int mul(int i, int j) {
return (i*j);
}
int dvd(int i, int j) {
return (i/j);
}
int mod(int i, int j) {
return i%j;
}
int main() {
int i, a, b;
for (;;) {
printf("Enter a value between -1 and 4 and two ints\n");
printf("0: a + b \n 1: a - b \n 2: a * b \n 3: a / b \n 4: a mod b\n -1 a b to exit\n");
scanf("%d%d%d",&i, &a, &b);
if (i == -1)
break;
while (i >= NOP || i < -1) {
printf("Enter a value between -1 and 4 and two ints\n");
printf("0: a + b \n 1: a - b \n 2: a * b \n 3: a / b \n 4: a mod b\n -1 a b to exit\n");
scanf("%d%d%d", &i, &a, &b);
}
if (b == 0 && i >= DIV) {
printf("Cannot divide or calculate remainder if divisor is 0\n");
exit(EXIT_FAILURE);
}
printf("Result is %d\n", calculate(i, a, b));
}
printf("Exiting. Bye...\n");
exit(EXIT_SUCCESS);
}