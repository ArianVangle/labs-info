#ifndef ALGEBRA_H
#define ALGEBRA_H

#include "matrix.h"

typedef struct {
    int value;
} Integer;

typedef struct {
    int re;
    int im;
} Complex;

const AlgebraOperations* GetIntegerOps(void);
const AlgebraOperations* GetComplexOps(void);

Matrix* create_integer_matrix(int size, const int* values);
Matrix* create_complex_matrix(int size, const int* re_vals, const int* im_vals);

int integer_matrices_equal(const Matrix* m1, const Matrix* m2);
int complex_matrices_equal(const Matrix* m1, const Matrix* m2);

#endif // ALGEBRA_H