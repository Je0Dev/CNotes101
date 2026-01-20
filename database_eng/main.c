#include "btree.h"
#include <stdio.h>
#include <string.h>

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(void) {
    BTree *db = btree_create("my_database.db");
    if (!db) return 1;

    int choice, key;
    KeyValue item, result;
    
    printf("B-Tree Database Engine (T=%d)\n", T);

    while (1) {
        printf("\n1. Put (Insert/Update)\n2. Get (Search)\n3. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            clear_stdin();
            printf("Invalid.\n");
            continue;
        }
        clear_stdin();

        switch(choice) {
            case 1:
                printf("Enter Key (int): ");
                scanf("%d", &item.key);
                clear_stdin();
                printf("Enter Value (string): ");
                fgets(item.value, 100, stdin);
                item.value[strcspn(item.value, "\n")] = 0;
                
                // Note: Real B-Tree 'insert' handles updates.
                // This simple one doesn't. We'll just insert.
                btree_insert(db, item);
                printf("Inserted key %d.\n", item.key);
                break;
            case 2:
                printf("Enter Key to find: ");
                scanf("%d", &key);
                clear_stdin();
                
                if (btree_search(db, key, &result)) {
                    printf("Found -> Key: %d, Value: '%s'\n", result.key, result.value);
                } else {
                    printf("Key %d not found.\n", key);
                }
                break;
            case 3:
                btree_close(db);
                printf("Database closed.\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
