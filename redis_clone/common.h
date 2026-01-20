/* common.h - Shared includes and definitions */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h> // C11 Atomics
#include <unistd.h>
#include <errno.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// --- Debugging ---
// Enable this for verbose output
// #define DEBUG_LOG

#ifdef DEBUG_LOG
#define LOG(...) do { fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#else
#define LOG(...) do {} while(0)
#endif

#define ERROR_EXIT(...) do { fprintf(stderr, "[ERROR %s:%d] ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, ": %s\n", strerror(errno)); exit(EXIT_FAILURE); } while(0)

#endif // COMMON_H

