/* chunk.h - Defines the bytecode instructions */
#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "value.h" // A chunk has a constant pool (ValueArray)

// --- The Bytecode ---
// These are the "opcodes" our VM will execute.
typedef enum {
    OP_CONSTANT, // Push a constant from the pool
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NEGATE,   // Unary -
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_RETURN,   // Return from a function (or end script)
} OpCode;

/*
 * A Chunk is a dynamic array of bytecode.
 * It also stores the "constant pool" (literals like numbers).
 */
typedef struct {
    int count;
    int capacity;
    uint8_t* code;     // The bytecode
    ValueArray constants; // The constant pool
    int* lines;        // Line numbers (for error reporting)
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);

#endif
