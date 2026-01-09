/* Αρχείο επικεφαλίδας fifoq_arr.h
Προγραμματιστική διεπαφή για τη FIFO
αποθήκη χαρακτήρων */
#ifndef FIFOQ_H
#define FIFOQ_H
#include <stdbool.h>
#define N 100
typedef struct {
/* πίνακας για αποθήκευση χαρακτήρων */
char buf[N];
/* θέση τοποθέτησης χαρακτήρα */
int in;
/* θέση για απομάκρυνση χαρακτήρα */
int out;
/* αριθμός στοιχείων "υπό χρήση" */
int occupied;
} FIFO_Queue;

/* αρχικοποιεί την ουρά */
void fifoq_init(FIFO_Queue *q);
/* αν η ουρά έχει χώρο, τοποθετεί την τιμή c
και επιστρέφει 1, διαφορετικά επιστρέφει 0
*/
int fifoq_put(FIFO_Queue *q, char c);
/* επιστρέφει 1 αν η ουρά είναι άδεια,
διαφορετικά επιστρέφει 0 */
bool fifoq_empty(const FIFO_Queue *q);
/* ορίζεται μόνο όταν η ουρά δεν είναι κενή!
απομακρύνει και επιστρέφει την επόμενη τιμή
που βρίσκεται αποθηκευμένη στην ουρά */
char fifoq_get(FIFO_Queue *q);
#endif
