#include "vm.h"
#include "common.h"
#include "debug.h"
#include "compiler.h"
#include <stdio.h>
#include <stdarg.h> // for va_list

VM vm; // Global VM object

// --- Stack ---
static void resetStack() {
    vm.stackTop = vm.stack;
}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

static Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}

// --- Runtime Error ---
static void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    // Show stack trace
    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

// --- Public API ---
void initVM() {
    resetStack();
}
void freeVM() {
    // We will need this when we add objects
}

/**
 * @brief The main VM execution loop.
 * This is the "heartbeat" of the interpreter.
 */
static InterpretResult run() {
    // --- Niche C: Register Caching ---
    // We copy ip to a local var. Many C compilers
    // will place this in a CPU register, making the
    // loop *much* faster than "vm.ip".
    register uint8_t* ip = vm.ip;

    // --- Helper Macros ---
    #define READ_BYTE() (*ip++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    
    // Niche C: This is a "binary" operator macro
    // It pops b, then a, performs the op, and pushes the result.
    #define BINARY_OP(valueType, op) \
        do { \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
                runtimeError("Operands must be numbers."); \
                return INTERPRET_RUNTIME_ERROR; \
            } \
            double b = AS_NUMBER(pop()); \
            double a = AS_NUMBER(pop()); \
            push(valueType(a op b)); \
        } while (false)

    // --- The Dispatch Loop ---
    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
        // Print the stack
        printf("          ");
        for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        // Print the instruction
        disassembleInstruction(vm.chunk, (int)(ip - vm.chunk->code));
        #endif

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_NIL: push(NIL_VAL); break;
            case OP_TRUE: push(BOOL_VAL(true)); break;
            case OP_FALSE: push(BOOL_VAL(false)); break;

            case OP_NEGATE:
                if (!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;

            case OP_ADD:      BINARY_OP(NUMBER_VAL, +); break;
            case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;

            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                vm.ip = ip; // Write back the IP
                return INTERPRET_OK;
            }
        }
    }
    
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();
    
    freeChunk(&chunk);
    return result;
}
