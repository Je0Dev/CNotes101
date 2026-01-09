#include <stdio.h>
#include <stdlib.h>
#define SIZE 5
typedef struct node
{
int key;
struct node *next;
} node;
int hash_func(int key);
void insert_key(node **cur, int key);
node* find_key(node *p, int key);
void free_mem(node *hash_arr[]);
int main(void)
{
node *p, *hash_arr[SIZE] = {NULL};
int i, pos, key;
for(i = 0; i < 10; i++) /* Insert 10 keys. */
{
printf("Enter key: ");
scanf("%d", &key);
pos = hash_func(key);
insert_key(&hash_arr[pos], key);
}
printf("Enter key to search: ");
scanf("%d", &key);
pos = hash_func(key);
p = find_key(hash_arr[pos], key);
if(p == NULL)
printf("Key_%d doesn't exist\n", key);
else
printf("Key_%d exists\n", key);
free_mem(hash_arr);
return 0;
}
int hash_func(int key) /* We write it separately from the rest of the
code to make it clear. If we want to use another hashing function, we
just change that line. */
{
return key % SIZE;
}
void insert_key(node **cur, int key)
{
node *prev, *tmp, *p;
tmp = (node*) malloc(sizeof(node));
if(tmp == NULL)
{
printf("Error: Not available memory\n");
exit(EXIT_FAILURE);
}
tmp->key = key;
tmp->next = NULL;
if(*cur == NULL) /* We check if it is the first time to insert a
key in that position. If it isn't, we add the new node at the end of the
list. */

*cur = tmp;
else
{
p = prev = *cur;
while(p != NULL)
{
prev = p;
p = p->next;
}
p = tmp;
prev->next = p;
}
}
node* find_key(node *p, int key)
{
while(p != NULL)
{
if(p->key == key)
return p;
p = p->next;
}
}
void free_mem(node *hash_arr[])
{
int i;
node *p, *tmp;
for(i = 0; i < SIZE; i++)
{
p = hash_arr[i];
printf("\nKeys_%d: ", i); /* Display the key values. */
while(p != NULL)
{
printf("%d ", p->key);
tmp = p->next;
free(p);
p = tmp;
}
}
}