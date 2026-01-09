#include <stdio.h>
#include <stdlib.h>
#define N 10
void populateArray(int *, size_t, int
(*f)(void));
int getNextRandomValue(void);
int getNextRandomEvenNum(void);
void populateArray(int *array,
size_t arraySize,
int (*getNextValue)(void))
{
for (size_t i=0; i<arraySize; i++)
array[i] = getNextValue();

}
int getNextRandomValue(void) {
return rand();
}
/* H getNextRandomEvenNum μπορεί να αντικαταστήσει τη
* getNextRandomValue στην παραγωγή τυχαίων αριθμών για τα
* στοιχεία του πίνακα χωρίς να αλλάξει ο υπόλοιπος κώδικας */
int getNextRandomEvenNum(void) {
int x;
while ((x = rand())%2)
continue;
return x;
}

int main(void){
int i, myarray[N];
srand(time(0));
populateArray(myarray,
(size_t) N,
getNextRandomValue);
printf("Printing array's values: ");
for (i=0; i<N; i++)
printf("%d ", myarray[i]);
printf("\n");
exit(EXIT_SUCCESS);
}