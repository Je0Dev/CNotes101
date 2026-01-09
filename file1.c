#include <stdio.h>
#include <stdlib.h>
int main(void) {
#define SIZE 10
double A[SIZE] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
// Εγγραφή του Α σε αρχείο
FILE * fp = fopen("test.bin", "wb");
fwrite(A, sizeof(double), SIZE, fp);
fclose (fp);
// Ανάγνωση των τιμών double στον πίνακα Β
double B[SIZE];
fp = fopen("test.bin", "rb");
// Η θέση αρχείου τίθεται μπροστά από την τρίτη τιμή double
if (fseek(fp, sizeof(double) * 2L, SEEK_SET) != 0) {
fprintf(stderr, "fseek() failed \n");
fclose(fp);
return EXIT_FAILURE;
}
int ret_code = fread(B, sizeof(double), 8, fp); // ανάγνωση 8 τιμών double
printf("ret_code == %d\n", ret_code); // εμφάνιση των τιμών που διαβάστηκαν
printf("B[0] == %.1f, B[7] == %.1f\n", B[0], B[7]);
fclose(fp);
return EXIT_SUCCESS;
}
