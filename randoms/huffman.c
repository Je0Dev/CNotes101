/*
 * A simplified Huffman Coding implementation for demonstration.
 * This version only handles ASCII characters (0-127) and does not
 * store the tree in the compressed file. It's focused on the
 * algorithm itself.
 *
 * To use:
 * 1. Create 'input.txt'
 * 2. gcc huffman.c -o huffman
 * 3. ./huffman
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 128 // For ASCII

// --- Data Structures ---

// 1. Huffman Tree Node
typedef struct Node {
    char data;
    unsigned freq;
    struct Node *left, *right;
} Node;

// 2. Min-Heap
typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    Node** array; // Array of Node pointers
} MinHeap;

// --- Function Prototypes ---
Node* buildHuffmanTree(char data[], int freq[], int size);
void generateCodes(Node* root, int arr[], int top, char* huffCodes[]);
void compressFile(const char* inFile, const char* outFile, char* huffCodes[]);
void decompressFile(const char* inFile, const char* outFile, Node* root);
void calculateFreq(const char* filename, int freq[]);

// --- Helper Functions ---
Node* newNode(char data, unsigned freq) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (Node**)malloc(minHeap->capacity * sizeof(Node*));
    return minHeap;
}

void swapNodes(Node** a, Node** b) {
    Node* t = *a;
    *a = *b;
    *b = t;
}

// Standard min-heapify function
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

Node* extractMin(MinHeap* minHeap) {
    Node* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, Node* node) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) {
    MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    
    // Build heap
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
        
    return minHeap;
}

// --- Core Logic ---

Node* buildHuffmanTree(char data[], int freq[], int size) {
    Node *left, *right, *top;
    MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    // Build tree
    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        
        // '$' is a special value for internal nodes
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// Recursive helper to generate codes
void generateCodes(Node* root, int arr[], int top, char* huffCodes[]) {
    if (root->left) {
        arr[top] = 0;
        generateCodes(root->left, arr, top + 1, huffCodes);
    }
    if (root->right) {
        arr[top] = 1;
        generateCodes(root->right, arr, top + 1, huffCodes);
    }
    // If this is a leaf node
    if (!root->left && !root->right) {
        // Malloc space for the code string
        huffCodes[(int)root->data] = (char*)malloc(sizeof(char) * (top + 1));
        int i;
        for (i = 0; i < top; ++i)
            huffCodes[(int)root->data][i] = arr[i] + '0'; // '0' or '1'
        huffCodes[(int)root->data][i] = '\0';
    }
}

// Read file and count frequencies
void calculateFreq(const char* filename, int freq[]) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening input file");
        exit(1);
    }
    for(int i = 0; i < MAX_CHARS; i++) freq[i] = 0;
    
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if(c < MAX_CHARS)
            freq[c]++;
    }
    fclose(fp);
}

// (Simplified) Compress function
// Does not handle bit-packing, just writes code strings.
// A real version would pack 8 codes into a byte.
void compressFile(const char* inFile, const char* outFile, char* huffCodes[]) {
    FILE* in = fopen(inFile, "r");
    FILE* out = fopen(outFile, "w"); // "wb" for real bit-packing
    if(!in || !out) {
        perror("File error");
        exit(1);
    }
    
    int c;
    while((c = fgetc(in)) != EOF) {
        if(c < MAX_CHARS && huffCodes[c]) {
            fputs(huffCodes[c], out);
        }
    }
    fclose(in);
    fclose(out);
}

// (Simplified) Decompress function
void decompressFile(const char* inFile, const char* outFile, Node* root) {
    FILE* in = fopen(inFile, "r");
    FILE* out = fopen(outFile, "w");
    if(!in || !out) {
        perror("File error");
        exit(1);
    }
    
    Node* current = root;
    int c;
    while((c = fgetc(in)) != EOF) {
        // c is '0' or '1'
        if(c == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        // If it's a leaf node
        if(!current->left && !current->right) {
            fputc(current->data, out);
            current = root; // Go back to root for next code
        }
    }
    fclose(in);
    fclose(out);
}

// --- Main Driver ---
int main() {
    int freq[MAX_CHARS];
    char* huffCodes[MAX_CHARS] = {0}; // Array of string pointers
    int codeArr[MAX_CHARS];
    char charArr[MAX_CHARS];
    int uniqueChars = 0;

    // 1. Create a dummy input file
    FILE* fp = fopen("input.txt", "w");
    fputs("BCAADDDCCACACAC", fp);
    fclose(fp);

    // 2. Calculate frequencies
    calculateFreq("input.txt", freq);

    // 3. Get the set of unique chars and their freqs
    for (int i = 0; i < MAX_CHARS; i++) {
        if (freq[i] > 0) {
            charArr[uniqueChars] = (char)i;
            // This is a bit of a hack:
            // we pass the frequencies in a separate array
            // that maps to charArr.
            // A better way would be an array of structs.
            int* freqArr = (int*)realloc(NULL, (uniqueChars + 1) * sizeof(int));
            for(int j=0; j<uniqueChars; j++) freqArr[j] = freq[(int)charArr[j]];
            freqArr[uniqueChars] = freq[i];

            uniqueChars++;
            // This is inefficient, but good for demo
            if (uniqueChars > 1) {
                 int* tempFreq = (int*)malloc(uniqueChars * sizeof(int));
                 for(int j=0; j<uniqueChars; j++) tempFreq[j] = freq[(int)charArr[j]];
                 free(freqArr);
                 freqArr = tempFreq;
            }
        }
    }
    // Need to get the final frequency array
    int* finalFreq = (int*)malloc(uniqueChars * sizeof(int));
    for(int j=0; j<uniqueChars; j++) finalFreq[j] = freq[(int)charArr[j]];


    // 4. Build the Huffman Tree
    Node* root = buildHuffmanTree(charArr, finalFreq, uniqueChars);

    // 5. Generate codes
    generateCodes(root, codeArr, 0, huffCodes);

    printf("--- Huffman Codes ---\n");
    for (int i = 0; i < MAX_CHARS; i++) {
        if (huffCodes[i]) {
            printf("'%c': %s\n", (char)i, huffCodes[i]);
        }
    }

    // 6. Compress
    compressFile("input.txt", "compressed.txt", huffCodes);
    printf("\n'input.txt' compressed to 'compressed.txt'\n");

    // 7. Decompress
    decompressFile("compressed.txt", "decompressed.txt", root);
    printf("'compressed.txt' decompressed to 'decompressed.txt'\n");

    // Free all malloc'd strings
    for (int i = 0; i < MAX_CHARS; i++) {
        if(huffCodes[i]) free(huffCodes[i]);
    }
    // Free tree (not implemented, but should be)
    // free_tree(root); 
    free(finalFreq);
    
    return 0;
}
