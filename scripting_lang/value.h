/* value.h - Defines the Lox "Value" (a tagged union) */
#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"

// --- The Tagged Union ---
// This is the core C "niche" trick.
// A single Value can be a bool, nil, or number.
typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
    } as; // The "as" union
} Value;

// --- Macros to check and create Values ---
#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)

#define BOOL_VAL(value)   ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL           ((Value){VAL_NIL, {.number = 0}}) // 'as' is ignored
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})


// --- Dynamic Array (for constants) ---
// This is our own "std::vector<Value>"
typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);

void printValue(Value value);

#endif
