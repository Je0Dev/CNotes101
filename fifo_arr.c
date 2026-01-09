#include "fifo_arr.h"
void fifoq_init(FIFO_Queue *q) {
q->in=0;
q->out=0;
q->occupied=0;
}
int fifoq_put(FIFO_Queue *q, char c) {
if (q->occupied == N) {
return(0);
}
q->buf[q->in]=c;
q->in=(q->in+1)%N;
q->occupied++;
return(1);
}
bool fifoq_empty(const FIFO_Queue *q) {
return(q->occupied==0);
}
char fifoq_get(FIFO_Queue *q) {
char c;
c=q->buf[q->out];
q->out=(q->out+1)%N;
q->occupied--;
return(c);
}