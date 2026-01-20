/* chunk.c - Implementation of the Chunk dynamic array */
#include "chunk.h"
#include <stdlib.h>

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    free(chunk->code);
    free(chunk->lines);
    freeValueArray(&chunk->constants);
    initChunk(chunk); // Zero out the struct
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = (oldCapacity < 8) ? 8 : oldCapacity * 2;
        chunk->code = (uint8_t*)realloc(chunk->code, sizeof(uint8_t) * chunk->capacity);
        chunk->lines = (int*)realloc(chunk->lines, sizeof(int) * chunk->capacity);
        if (chunk->code == NULL || chunk->lines == NULL) {
            perror("realloc Chunk");
            exit(1);
        }
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
    // Write the value to the constant pool
    writeValueArray(&chunk->constants, value);
    // Return the index where it was stored
    return chunk->constants.count - 1;
}
