#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ALPHABET_SIZE 26

struct GeeksForGeeksTrieNode {
    struct GeeksForGeeksTrieNode *children[ALPHABET_SIZE];
    bool isendofword;
};

typedef struct GeeksForGeeksTrieNode GeeksForGeeksTrieNode;

GeeksForGeeksTrieNode *createNode() {
    GeeksForGeeksTrieNode *node = (GeeksForGeeksTrieNode *)malloc(sizeof(GeeksForGeeksTrieNode));
    node->isendofword = false;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void insert(GeeksForGeeksTrieNode *root, const char *key) {
    GeeksForGeeksTrieNode *current = root;
    for (int i = 0; i < strlen(key); i++) {
        int index = key[i] - 'a';
        if (current->children[index] == NULL) {
            current->children[index] = createNode();
        }
        current = current->children[index];
    }
    current->isendofword = true;
}

bool search(GeeksForGeeksTrieNode *root, const char *key) {
    GeeksForGeeksTrieNode *current = root;
    for (int i = 0; i < strlen(key); i++) {
        int index = key[i] - 'a';
        if (current->children[index] == NULL) {
            return false;
        }
        current = current->children[index];
    }
    return (current != NULL && current->isendofword);
}

bool isempty(GeeksForGeeksTrieNode *root) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            return false;
        }
    }
    return true;
}

GeeksForGeeksTrieNode *deleteHelper(GeeksForGeeksTrieNode *root, const char *key, int depth) {
    if (root == NULL) {
        return NULL;
    }
    if (depth == strlen(key)) {
        if (root->isendofword) {
            root->isendofword = false;
        }
        if (isempty(root)) {
            free(root);
            root = NULL;
        }
        return root;
    }
    int index = key[depth] - 'a';
    root->children[index] = deleteHelper(root->children[index], key, depth + 1);
    if (isempty(root) && !root->isendofword) {
        free(root);
        root = NULL;
    }
    return root;
}

void deletekey(GeeksForGeeksTrieNode *root, const char *key) {
    deleteHelper(root, key, 0);
}

int main() {
    GeeksForGeeksTrieNode *root = createNode();

    insert(root, "hello");
    insert(root, "world");

    printf("%s\n", search(root, "hello") ? "Found" : "Not Found");
    printf("%s\n", search(root, "world") ? "Found" : "Not Found");
    printf("%s\n", search(root, "geeks") ? "Found" : "Not Found");

    deletekey(root, "hello");
    printf("%s\n", search(root, "hello") ? "Found" : "Not Found");

    return 0;
}