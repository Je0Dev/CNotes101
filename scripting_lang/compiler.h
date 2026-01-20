/* compiler.h - The Bytecode Compiler */
#ifndef CLOX_COMPILER_H
#define CLOX_COMPILER_H

#include "vm.h" // For Chunk
#include "lexer.h"

// --- Parser Struct ---
// Holds the state of the compiler
typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

// --- Precedence ---
// For our Pratt Parser
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} Precedence;

// Function pointer for a parsing rule
typedef void (*ParseFn)();

// A rule for the Pratt parser
typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

/**
 * @brief Main compiler function.
 * @param source The C-Script source code string.
 * @param chunk The bytecode chunk to compile into.
 * @return true on success, false on error.
 */
bool compile(const char* source, Chunk* chunk);

#endif

