/* test_extern1.c */
#include <stdio.h>

extern int i;
void incval();
int main(int argc, char *argv[])
{
i=15;
printf("%d\n",i);
incval();
printf("%d\n",i);
}