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

    ErrorCode (*lu_decompose_fn)(const Matrix* A, Matrix* L, Matrix* U);

} AlgebraOperations;

const AlgebraOperations* GetIntegerOps(void);
const AlgebraOperations* GetComplexOps(void);
const AlgebraOperations* GetDoubleOps(void);

ErrorCode test_ring_axioms(const AlgebraOperations* ops);
const char* error_message(ErrorCode code);

#endif  // ALGEBRA_H