#include "algebra.h"

#include <stdlib.h>
#include <string.h>

static void integer_add(const void* e1, const void* e2, void* result) {
    ((Integer*)result)->value =
        ((const Integer*)e1)->value + ((const Integer*)e2)->value;
}

static void integer_multiply(const void* e1, const void* e2, void* result) {
    ((Integer*)result)->value =
        ((const Integer*)e1)->value * ((const Integer*)e2)->value;
}

static void complex_add(const void* e1, const void* e2, void* result) {
    ((Complex*)result)->re =
        ((const Complex*)e1)->re + ((const Complex*)e2)->re;
    ((Complex*)result)->im =
        ((const Complex*)e1)->im + ((const Complex*)e2)->im;
}

static void complex_multiply(const void* e1, const void* e2, void* result) {
    const Complex* z1 = (const Complex*)e1;
    const Complex* z2 = (const Complex*)e2;
    Complex* res = (Complex*)result;

    int new_re = z1->re * z2->re - z1->im * z2->im;
    int new_im = z1->re * z2->im + z1->im * z2->re;

    res->re = new_re;
    res->im = new_im;
}

static AlgebraOperations* IntegerOpsInstance = NULL;
static AlgebraOperations* ComplexOpsInstance = NULL;

const AlgebraOperations* GetIntegerOps(void) {
    if (IntegerOpsInstance == NULL) {
        IntegerOpsInstance = malloc(sizeof(AlgebraOperations));
        if (IntegerOpsInstance) {
            IntegerOpsInstance->addFn = integer_add;
            IntegerOpsInstance->multiplyFn = integer_multiply;
        }
    }
    return IntegerOpsInstance;
}

const AlgebraOperations* GetComplexOps(void) {
    if (ComplexOpsInstance == NULL) {
        ComplexOpsInstance = malloc(sizeof(AlgebraOperations));
        if (ComplexOpsInstance) {
            ComplexOpsInstance->addFn = complex_add;
            ComplexOpsInstance->multiplyFn = complex_multiply;
        }
    }
    return ComplexOpsInstance;
}

Matrix* create_integer_matrix(int size, const int* values) {
    Matrix* m = create_matrix(size, GetIntegerOps(), sizeof(Integer));
    if (!m || !values) return m;

    Integer* data = (Integer*)m->data;
    for (int i = 0; i < size * size; i++) {
        data[i].value = values[i];
    }
    return m;
}

Matrix* create_complex_matrix(int size, const int* re_vals,
                              const int* im_vals) {
    Matrix* m = create_matrix(size, GetComplexOps(), sizeof(Complex));
    if (!m) return m;

    Complex* data = (Complex*)m->data;
    for (int i = 0; i < size * size; i++) {
        data[i].re = re_vals ? re_vals[i] : 0;
        data[i].im = im_vals ? im_vals[i] : 0;
    }
    return m;
}

int integer_matrices_equal(const Matrix* m1, const Matrix* m2) {
    if (!m1 || !m2 || m1->size != m2->size) return 0;
    Integer* d1 = (Integer*)m1->data;
    Integer* d2 = (Integer*)m2->data;
    for (int i = 0; i < m1->size * m1->size; i++) {
        if (d1[i].value != d2[i].value) return 0;
    }
    return 1;
}

int complex_matrices_equal(const Matrix* m1, const Matrix* m2) {
    if (!m1 || !m2 || m1->size != m2->size) return 0;
    Complex* d1 = (Complex*)m1->data;
    Complex* d2 = (Complex*)m2->data;
    for (int i = 0; i < m1->size * m1->size; i++) {
        if (d1[i].re != d2[i].re || d1[i].im != d2[i].im) return 0;
    }
    return 1;
}