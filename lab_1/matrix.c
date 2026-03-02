#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Matrix* create_matrix(int size, const AlgebraOperations* ops,
                      size_t elem_size) {
    if (size <= 0 || ops == NULL || elem_size == 0) {
        fprintf(stderr, "Error: invalid matrix parameters\n");
        return NULL;
    }

    Matrix* m = malloc(sizeof(Matrix));
    if (!m) {
        fprintf(stderr, "Error: malloc failed for Matrix\n");
        return NULL;
    }

    m->size = size;
    m->operations = (AlgebraOperations*)ops;
    m->element_size = elem_size;
    m->data = malloc(elem_size * size * size);

    if (!m->data) {
        fprintf(stderr, "Error: malloc failed for matrix data\n");
        free(m);
        return NULL;
    }

    return m;
}

void destroy_matrix(Matrix* m) {
    if (m) {
        free(m->data);
        free(m);
    }
}

void matrix_add(const Matrix* m1, const Matrix* m2, Matrix* result) {
    if (!m1 || !m2 || !result) {
        fprintf(stderr, "Error: NULL matrix pointer\n");
        return;
    }

    if (m1->size != m2->size || m1->size != result->size) {
        fprintf(stderr, "Error: matrix size mismatch (%d vs %d vs %d)\n",
                m1->size, m2->size, result->size);
        return;
    }

    if (m1->operations != m2->operations ||
        m1->operations != result->operations) {
        fprintf(stderr, "Error: incompatible matrix types\n");
        return;
    }

    for (int i = 0; i < m1->size * m1->size; i++) {
        m1->operations->addFn((char*)m1->data + i * m1->element_size,
                              (char*)m2->data + i * m2->element_size,
                              (char*)result->data + i * result->element_size);
    }
}

void matrix_multiply_scalar(const Matrix* m, const void* scalar,
                            Matrix* result) {
    if (!m || !result || !scalar) {
        fprintf(stderr, "Error: NULL pointer in multiply_scalar\n");
        return;
    }

    if (m->size != result->size) {
        fprintf(stderr, "Error: matrix size mismatch\n");
        return;
    }

    if (m->operations != result->operations) {
        fprintf(stderr, "Error: incompatible matrix types\n");
        return;
    }

    for (int i = 0; i < m->size * m->size; i++) {
        m->operations->multiplyFn(
            (char*)m->data + i * m->element_size, scalar,
            (char*)result->data + i * result->element_size);
    }
}

void matrix_multiply(const Matrix* A, const Matrix* B, Matrix* result) {
    if (!A || !B || !result) {
        fprintf(stderr, "Error: NULL matrix pointer\n");
        return;
    }

    if (A->size != B->size || A->size != result->size) {
        fprintf(stderr, "Error: matrix size mismatch\n");
        return;
    }

    if (A->operations != B->operations || A->operations != result->operations) {
        fprintf(stderr, "Error: incompatible matrix types\n");
        return;
    }

    int n = A->size;
    size_t elem_size = A->element_size;
    AlgebraOperations* ops = A->operations;

    void* accumulator = malloc(elem_size);
    void* temp_product = malloc(elem_size);

    if (!accumulator || !temp_product) {
        fprintf(stderr, "Error: malloc failed for temporary buffers\n");
        free(accumulator);
        free(temp_product);
        return;
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
}

typedef struct {
    int value;
} Integer;
typedef struct {
    int re, im;
} Complex;

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