#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

struct AlgebraOperations;

typedef struct Matrix {
    void* data;
    int size;
    struct AlgebraOperations* operations;
    ssize_t element_size;
} Matrix;

typedef struct AlgebraOperations {
    void (*addFn)(const void*, const void*, void*);
    void (*multiplyFn)(const void*, const void*, void*);
} AlgebraOperations;

Matrix* create_matrix(int size, const AlgebraOperations* ops, size_t elem_size);
void destroy_matrix(Matrix* m);

void matrix_add(const Matrix* m1, const Matrix* m2, Matrix* result);
void matrix_multiply_scalar(const Matrix* m, const void* scalar,
                            Matrix* result);
void matrix_multiply(const Matrix* A, const Matrix* B, Matrix* result);

void print_integer_matrix(const Matrix* m, const char* name);
void print_complex_matrix(const Matrix* m, const char* name);

#endif  // MATRIX_H