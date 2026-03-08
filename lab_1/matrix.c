#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "algebra.h"

Matrix* create_matrix(int size, const AlgebraOperations* ops,
                      size_t elem_size) {
    if (size <= 0 || ops == NULL || elem_size == 0) {
        return NULL;
    }

    Matrix* m = malloc(sizeof(Matrix));
    if (!m) {
        return NULL;
    }

    m->size = size;
    m->operations = (AlgebraOperations*)ops;
    m->element_size = elem_size;
    m->data = malloc(elem_size * size * size);

    if (!m->data) {
        free(m);
        return NULL;
    }

    return m;
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

Matrix* create_double_matrix(int size, const double* values) {
    Matrix* m = create_matrix(size, GetDoubleOps(), sizeof(Double));
    if (!m || !values) return m;

    Double* data = (Double*)m->data;
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

void destroy_matrix(Matrix* m) {
    if (m) {
        free(m->data);
        free(m);
    }
}

ErrorCode matrix_add(const Matrix* m1, const Matrix* m2, Matrix* result) {
    if (!m1 || !m2 || !result) {
        return ERR_NULL_POINTER;
    }

    if (m1->size != m2->size || m1->size != result->size) {
        return ERR_SIZE_MISMATCH;
    }

    if (m1->operations != m2->operations ||
        m1->operations != result->operations) {
        return ERR_TYPE_MISMATCH;
    }

    for (int i = 0; i < m1->size * m1->size; i++) {
        m1->operations->addFn((char*)m1->data + i * m1->element_size,
                              (char*)m2->data + i * m2->element_size,
                              (char*)result->data + i * result->element_size);
    }
    return ERR_OK;
}

ErrorCode matrix_multiply_scalar(const Matrix* m, const void* scalar,
                            Matrix* result) {
    if (!m || !result || !scalar) {
        return ERR_NULL_POINTER;
    }

    if (m->size != result->size) {
        return ERR_SIZE_MISMATCH;
    }

    if (m->operations != result->operations) {
        return ERR_TYPE_MISMATCH;
    }

    for (int i = 0; i < m->size * m->size; i++) {
        m->operations->multiplyFn(
            (char*)m->data + i * m->element_size, scalar,
            (char*)result->data + i * result->element_size);
    }
    return ERR_OK;
}

ErrorCode matrix_multiply(const Matrix* A, const Matrix* B, Matrix* result) {
    if (!A || !B || !result) {
        return ERR_NULL_POINTER;
    }

    if (A->size != B->size || A->size != result->size) {
        return ERR_SIZE_MISMATCH;
    }

    if (A->operations != B->operations || A->operations != result->operations) {
        return ERR_TYPE_MISMATCH;
    }

    int n = A->size;
    size_t elem_size = A->element_size;
    AlgebraOperations* ops = A->operations;

    void* accumulator = malloc(elem_size);
    void* temp_product = malloc(elem_size);

    if (!accumulator || !temp_product) {
        free(accumulator);
        free(temp_product);
        return ERR_OUT_OF_MEMORY;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            void* a_i0 = (char*)A->data + (i * n + 0) * elem_size;
            void* b_0j = (char*)B->data + (0 * n + j) * elem_size;
            ops->multiplyFn(a_i0, b_0j, accumulator);

            for (int k = 1; k < n; k++) {
                void* a_ik = (char*)A->data + (i * n + k) * elem_size;
                void* b_kj = (char*)B->data + (k * n + j) * elem_size;
                ops->multiplyFn(a_ik, b_kj, temp_product);
                ops->addFn(accumulator, temp_product, accumulator);
            }

            void* c_ij = (char*)result->data + (i * n + j) * elem_size;
            memcpy(c_ij, accumulator, elem_size);
        }
    }

    free(accumulator);
    free(temp_product);
    return ERR_OK;
}

ErrorCode matrix_zero(Matrix* m) {
    if (!m || !m->operations->zeroFn) return ERR_NULL_POINTER;
    for (int i = 0; i < m->size * m->size; i++) {
        m->operations->zeroFn((char*)m->data + i * m->element_size);
    }
    return ERR_OK;
}

ErrorCode matrix_negate(const Matrix* m, Matrix* result) {
    if (!m || !result || !m->operations->negateFn) return ERR_NULL_POINTER;
    for (int i = 0; i < m->size * m->size; i++) {
        m->operations->negateFn(
            (char*)m->data + i * m->element_size,
            (char*)result->data + i * result->element_size
        );
    }
    return ERR_OK;
}

ErrorCode matrix_subtract(const Matrix* m1, const Matrix* m2, Matrix* result) {
    if (!m1 || !m2 || !result || !m1->operations->subtractFn) return ERR_NULL_POINTER;
    for (int i = 0; i < m1->size * m1->size; i++) {
        m1->operations->subtractFn(
            (char*)m1->data + i * m1->element_size,
            (char*)m2->data + i * m2->element_size,
            (char*)result->data + i * result->element_size
        );
    }
    return ERR_OK;
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

int double_matrices_equal(const Matrix* m1, const Matrix* m2, double epsilon) {
    if (!m1 || !m2 || m1->size != m2->size) return 0;
    if (m1->operations != m2->operations) return 0;

    Double* d1 = (Double*)m1->data;
    Double* d2 = (Double*)m2->data;
    for (int i = 0; i < m1->size * m1->size; i++) {
        if (fabs(d1[i].value - d2[i].value) > epsilon) return 0;
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


ErrorCode matrix_lu_decompose(const Matrix* A, Matrix* L, Matrix* U) {
    if (!A || !L || !U) {
        return ERR_NULL_POINTER;
    }

    if (A->size != L->size || A->size != U->size) {
        return ERR_SIZE_MISMATCH;
    }

    if (!A->operations || !A->operations->lu_decompose_fn) {
        return ERR_TYPE_MISMATCH;
    }

    return A->operations->lu_decompose_fn(A, L, U);
}

void print_integer_matrix(const Matrix* m, const char* name) {
    if (!m) return;
    printf("%s:\n", name);
    Integer* data = (Integer*)m->data;
    for (int i = 0; i < m->size; i++) {
        printf("  [");
        for (int j = 0; j < m->size; j++) {
            printf("%4d", data[i * m->size + j].value);
            if (j < m->size - 1) printf(",");
        }
        printf("]\n");
    }
}

void print_complex_matrix(const Matrix* m, const char* name) {
    if (!m) return;
    printf("%s:\n", name);
    Complex* data = (Complex*)m->data;
    for (int i = 0; i < m->size; i++) {
        printf("  [");
        for (int j = 0; j < m->size; j++) {
            int idx = i * m->size + j;
            printf("%+d%+di", data[idx].re, data[idx].im);
            if (j < m->size - 1) printf(", ");
        }
        printf("]\n");
    }
}

void print_double_matrix(const Matrix* m, const char* name) {
    if (!m || !m->data) return;

    if (m->operations != GetDoubleOps()) {
        return;
    }

    printf("%s:\n", name);
    Double* data = (Double*)m->data;

    for (int i = 0; i < m->size; i++) {
        printf("  [");
        for (int j = 0; j < m->size; j++) {
            printf("%10.4f", data[i * m->size + j].value);
            if (j < m->size - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }
}