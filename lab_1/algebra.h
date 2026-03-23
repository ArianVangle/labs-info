#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <stddef.h>

typedef enum {
    ERR_OK = 0,
    ERR_NULL_POINTER = -1,
    ERR_SIZE_MISMATCH = -2,
    ERR_SINGULAR_MATRIX = -3,
    ERR_UNSUPPORTED_OPERATION = -4,
    ERR_TYPE_MISMATCH = -5,
    ERR_OUT_OF_MEMORY = -6,
    ERR_INVALID_INPUT = -7
} ErrorCode;

typedef struct {
    int value;
} Integer;

typedef struct {
    double value;
} Double;

typedef struct {
    int re;
    int im;
} Complex;

typedef struct Matrix Matrix;

typedef struct AlgebraOperations {
    void (*addFn)(const void*, const void*, void*);
    void (*subtractFn)(const void*, const void*, void*);
    void (*negateFn)(const void*, void*);
    void (*zeroFn)(void*);

    void (*multiplyFn)(const void*, const void*, void*);
    void (*oneFn)(void*);

    int (*isZeroFn)(const void*);
    int (*isOneFn)(const void*);

    void (*divideFn)(const void* a, const void* b, void* result); 
    void (*sqrtFn)(const void* a, void* result);    
    void (*magnitudeFn)(const void* a, double* result);

} AlgebraOperations;

const AlgebraOperations* get_integer_ops(void);
const AlgebraOperations* get_complex_ops(void);
const AlgebraOperations* get_double_ops(void);


#endif  // ALGEBRA_H