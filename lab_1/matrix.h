#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

#include "algebra.h"

typedef struct Matrix {
    void* data;
    int size;
    struct AlgebraOperations* operations;
    size_t element_size;
} Matrix;

Matrix* create_matrix(int size, const AlgebraOperations* ops, size_t elem_size);
Matrix* create_integer_matrix(int size, const int* values);
Matrix* create_complex_matrix(int size, const int* re_vals, const int* im_vals);
Matrix* create_double_matrix(int size, const double* values);
void destroy_matrix(Matrix* m);

int integer_matrices_equal(const Matrix* m1, const Matrix* m2);
int complex_matrices_equal(const Matrix* m1, const Matrix* m2);
int double_matrices_equal(const Matrix* m1, const Matrix* m2, double epsilon);

ErrorCode matrix_add(const Matrix* m1, const Matrix* m2, Matrix* result);
ErrorCode matrix_subtract(const Matrix* m1, const Matrix* m2, Matrix* result);
ErrorCode matrix_negate(const Matrix* m, Matrix* result);
ErrorCode matrix_multiply(const Matrix* A, const Matrix* B, Matrix* result);
ErrorCode matrix_multiply_scalar(const Matrix* m, const void* scalar,
                                 Matrix* result);
ErrorCode matrix_zero(Matrix* m);

ErrorCode matrix_lu_decompose(const Matrix* A, Matrix* L, Matrix* U);
ErrorCode forward_substitution(const Matrix* L, const Matrix* b, Matrix* y);
ErrorCode backward_substitution(const Matrix* U, const Matrix* y, Matrix* x);
ErrorCode solve_lu(const Matrix* A, const Matrix* b, Matrix* x);

ErrorCode matrix_qr_decompose(const Matrix* A, Matrix* Q, Matrix* R);
ErrorCode solve_qr(const Matrix* A, const Matrix* b, Matrix* x);

void benchmark_lu_vs_qr(int size);


void print_integer_matrix(const Matrix* m, const char* name);
void print_complex_matrix(const Matrix* m, const char* name);
void print_double_matrix(const Matrix* m, const char* name);
void print_double_vector(const Matrix* v, const char* name);

#endif  // MATRIX_H