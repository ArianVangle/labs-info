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
    void (*add_fn)(const void*, const void*, void*);
    void (*subtract_fn)(const void*, const void*, void*);
    void (*negate_fn)(const void*, void*);
    void (*zero_fn)(void*);

    void (*multiply_fn)(const void*, const void*, void*);
    void (*one_fn)(void*);

    int (*is_zero_fn)(const void*);
    int (*is_one_fn)(const void*);

    void (*divide_fn)(const void* a, const void* b, void* result); 
    void (*sqrt_fn)(const void* a, void* result);    
    void (*magnitude_fn)(const void* a, double* result);
    void (*set_from_double_fn)(void* a, double value); 

} AlgebraOperations;

const AlgebraOperations* get_integer_ops(void);
const AlgebraOperations* get_complex_ops(void);
const AlgebraOperations* get_double_ops(void);


#endif  // ALGEBRA_H