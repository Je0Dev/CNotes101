/* Ταξινομεί τις παραμέτρους της γραμμής εντολών (argv) σε αύξουσα
λεξικογραφική σειρά, με χρήση της συνάρτησης compare() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int compare(const void *, const void *);
int main (int argc, char *argv[ ]) {
int i;
argv++;
argc--;
qsort((char *)argv, argc, sizeof(char *), compare);
for (i = 0; i < argc; ++i)
printf("%s\n", argv[i]);

}
int compare (const void *arg1, const void *arg2) {
return(strcmp(*(char **)arg1, *(char **)arg2));
}