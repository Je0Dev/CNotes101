#include <stdio.h>
#include "fifo_arr.h"
// Έστω ότι απαιτείται να υλοποιηθεί μια αποθήκη χαρακτήρων
// που διαβάζει δεδομένα από κάποια είσοδο και τα αποθηκεύει
// σε κάποια δομή δεδομένων με λογική FIFO

//gcc -c fifoq_arr.c -o fifoq_arr.o <enter>
//gcc -c fifoq_client.c -o fifoq_client.o <enter>
//gcc fifoq_arr.o fifoq_client.ο -o fifoq_test <enter>
int main() {
char c;
FIFO_Queue q;
fifoq_init(&q);
while ((c=getchar()) != '\n') {
fifoq_put(&q, c);
}
while (!fifoq_empty(&q)) {
c = fifoq_get(&q);
putchar(c);
}
putchar('\n');
}