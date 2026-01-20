/* value.c - Implementation of ValueArray and printValue */
#include "common.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h> // for realloc, free

void initValueArray(ValueArray* array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void writeValueArray(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = (oldCapacity < 8) ? 8 : oldCapacity * 2;
        array->values = (Value*)realloc(array->values, sizeof(Value) * array->capacity);
        if (array->values == NULL) {
            perror("realloc ValueArray");
            exit(1);
        }
    }
    
    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array) {
    free(array->values);
    initValueArray(array);
}

void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL:   printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL:    printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
    }
}
