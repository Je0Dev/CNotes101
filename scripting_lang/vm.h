/* vm.h - The Bytecode Virtual Machine */
#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

// --- The VM Struct ---
// This holds the entire state of the running program.
typedef struct {
    Chunk* chunk;      // The bytecode being run
    uint8_t* ip;       // Instruction Pointer
    
    Value stack[STACK_MAX];
    Value* stackTop;   // Points *just past* the last item
    
} VM;

// Result of running the VM
typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);

// Stack operations
void push(Value value);
Value pop();

#endif
