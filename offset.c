#include <stddef.h>

struct S {
char c;
double d;
};
int main(void) {
printf("Size of struct S is: %lld bytes\n", sizeof(struct S));
printf("First element is at offset %zu\n", offsetof(struct S, c));
printf("Second element is at offset %zu\n", offsetof(struct S, d));
}