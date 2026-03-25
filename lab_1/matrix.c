#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    Matrix* m = create_matrix(size, get_integer_ops(), sizeof(Integer));
    if (!m || !values) return m;

    Integer* data = (Integer*)m->data;
    for (int i = 0; i < size * size; i++) {
        data[i].value = values[i];
    }
    return m;
}

Matrix* create_double_matrix(int size, const double* values) {
    Matrix* m = create_matrix(size, get_double_ops(), sizeof(Double));
    if (!m || !values) return m;

    Double* data = (Double*)m->data;
    for (int i = 0; i < size * size; i++) {
        data[i].value = values[i];
    }
    return m;
}

Matrix* create_complex_matrix(int size, const int* re_vals,
                              const int* im_vals) {
    Matrix* m = create_matrix(size, get_complex_ops(), sizeof(Complex));
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
        m->operations->negateFn((char*)m->data + i * m->element_size,
                                (char*)result->data + i * result->element_size);
    }
    return ERR_OK;
}

ErrorCode matrix_subtract(const Matrix* m1, const Matrix* m2, Matrix* result) {
    if (!m1 || !m2 || !result || !m1->operations->subtractFn)
        return ERR_NULL_POINTER;
    for (int i = 0; i < m1->size * m1->size; i++) {
        m1->operations->subtractFn(
            (char*)m1->data + i * m1->element_size,
            (char*)m2->data + i * m2->element_size,
            (char*)result->data + i * result->element_size);
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
    if (!A || !L || !U) return ERR_NULL_POINTER;
    if (A->size != L->size || A->size != U->size) return ERR_SIZE_MISMATCH;
    if (A->operations != L->operations || A->operations != U->operations) {
        return ERR_TYPE_MISMATCH;
    }

    const AlgebraOperations* ops = A->operations;
    int n = A->size;
    size_t elem_size = A->element_size;

    for (int i = 0; i < n * n; i++) {
        ops->zeroFn((char*)L->data + i * elem_size);
        ops->zeroFn((char*)U->data + i * elem_size);
    }
    for (int i = 0; i < n; i++) {
        ops->oneFn((char*)L->data + (i * n + i) * elem_size);
    }

    for (int k = 0; k < n; k++) {
        for (int j = k; j < n; j++) {
            void* u_kj = (char*)U->data + (k * n + j) * elem_size;
            void* a_kj = (char*)A->data + (k * n + j) * elem_size;
            
            memcpy(u_kj, a_kj, elem_size);

            for (int m = 0; m < k; m++) {
                void* l_km = (char*)L->data + (k * n + m) * elem_size;
                void* u_mj = (char*)U->data + (m * n + j) * elem_size;
                void* prod = malloc(elem_size);
                void* temp = malloc(elem_size);
                
                ops->multiplyFn(l_km, u_mj, prod);
                ops->subtractFn(u_kj, prod, temp);
                memcpy(u_kj, temp, elem_size);
                
                free(prod);
                free(temp);
            }
        }

        void* u_kk = (char*)U->data + (k * n + k) * elem_size;
        if (ops->isZeroFn(u_kk)) {
            return ERR_SINGULAR_MATRIX;
        }

        for (int i = k + 1; i < n; i++) {
            void* l_ik = (char*)L->data + (i * n + k) * elem_size;
            void* a_ik = (char*)A->data + (i * n + k) * elem_size;
            
            memcpy(l_ik, a_ik, elem_size);

            for (int m = 0; m < k; m++) {
                void* l_im = (char*)L->data + (i * n + m) * elem_size;
                void* u_mk = (char*)U->data + (m * n + k) * elem_size;
                void* prod = malloc(elem_size);
                void* temp = malloc(elem_size);
                
                ops->multiplyFn(l_im, u_mk, prod);
                ops->subtractFn(l_ik, prod, temp);
                memcpy(l_ik, temp, elem_size);
                
                free(prod);
                free(temp);
            }

            void* u_kk = (char*)U->data + (k * n + k) * elem_size;
            void* temp = malloc(elem_size);
            ops->divideFn(l_ik, u_kk, temp);
            memcpy(l_ik, temp, elem_size);
            free(temp);
        }
    }

    return ERR_OK;
}

/* === Прямая подстановка: решает L * y = b ===
 * L — нижняя треугольная матрица с единицами на диагонали
 * b — вектор правой части (матрица size×1)
 * y — выходной вектор решения (матрица size×1)
 */
ErrorCode forward_substitution(const Matrix* L, const Matrix* b, Matrix* y) {
    if (!L || !b || !y) return ERR_NULL_POINTER;
    if (L->size != b->size || L->size != y->size) return ERR_SIZE_MISMATCH;
    if (L->operations != b->operations || L->operations != y->operations) {
        return ERR_TYPE_MISMATCH;
    }

    const AlgebraOperations* ops = L->operations;
    int n = L->size;
    size_t elem_size = L->element_size;

    for (int i = 0; i < n; i++) {
        void* sum = malloc(elem_size);
        ops->zeroFn(sum);

        for (int j = 0; j < i; j++) {
            void* l_ij = (char*)L->data + (i * n + j) * elem_size;
            void* y_j = (char*)y->data + j * elem_size;
            void* prod = malloc(elem_size);
            void* temp = malloc(elem_size);

            ops->multiplyFn(l_ij, y_j, prod);
            ops->addFn(sum, prod, temp);
            memcpy(sum, temp, elem_size);

            free(prod);
            free(temp);
        }

        void* b_i = (char*)b->data + i * elem_size;
        ops->subtractFn(b_i, sum, (char*)y->data + i * elem_size);

        free(sum);
    }

    return ERR_OK;
}

/* === Обратная подстановка: решает U * x = y ===
 * U — верхняя треугольная матрица
 * y — вектор правой части (матрица size×1)
 * x — выходной вектор решения (матрица size×1)
 */
ErrorCode backward_substitution(const Matrix* U, const Matrix* y, Matrix* x) {
    if (!U || !y || !x) return ERR_NULL_POINTER;
    if (U->size != y->size || U->size != x->size) return ERR_SIZE_MISMATCH;
    if (U->operations != y->operations || U->operations != x->operations) {
        return ERR_TYPE_MISMATCH;
    }

    const AlgebraOperations* ops = U->operations;
    int n = U->size;
    size_t elem_size = U->element_size;

    for (int i = n - 1; i >= 0; i--) {
        void* sum = malloc(elem_size);
        ops->zeroFn(sum);

        for (int j = i + 1; j < n; j++) {
            void* u_ij = (char*)U->data + (i * n + j) * elem_size;
            void* x_j = (char*)x->data + j * elem_size;
            void* prod = malloc(elem_size);
            void* temp = malloc(elem_size);

            ops->multiplyFn(u_ij, x_j, prod);
            ops->addFn(sum, prod, temp);
            memcpy(sum, temp, elem_size);

            free(prod);
            free(temp);
        }

        void* y_i = (char*)y->data + i * elem_size;
        void* temp = malloc(elem_size);
        ops->subtractFn(y_i, sum, temp);

        void* u_ii = (char*)U->data + (i * n + i) * elem_size;
        if (ops->isZeroFn(u_ii)) {
            free(sum);
            free(temp);
            return ERR_SINGULAR_MATRIX;
        }

        ops->divideFn(temp, u_ii, (char*)x->data + i * elem_size);

        free(sum);
        free(temp);
    }

    return ERR_OK;
}

/* === Решение СЛАУ через LU: A * x = b ===
 * 1. LU-разложение: A = L * U
 * 2. Прямой ход: L * y = b
 * 3. Обратный ход: U * x = y
 */
ErrorCode solve_lu(const Matrix* A, const Matrix* b, Matrix* x) {
    if (!A || !b || !x) return ERR_NULL_POINTER;
    if (A->size != b->size || A->size != x->size) return ERR_SIZE_MISMATCH;
    if (A->operations != b->operations || A->operations != x->operations) {
        return ERR_TYPE_MISMATCH; 
    }

    int n = A->size;
    const AlgebraOperations* ops = A->operations;
    size_t elem_size = A->element_size;

    Matrix* L = create_matrix(n, ops, elem_size);
    Matrix* U = create_matrix(n, ops, elem_size);
    Matrix* y = create_matrix(n, ops, elem_size);

    if (!L || !U || !y) {
        destroy_matrix(L); destroy_matrix(U); destroy_matrix(y);
        return ERR_OUT_OF_MEMORY;
    }

    ErrorCode lu = matrix_lu_decompose(A, L, U);
    if (lu != ERR_OK) {
        destroy_matrix(L); destroy_matrix(U); destroy_matrix(y);
        return lu;
    }

    ErrorCode fwd = forward_substitution(L, b, y);
    if (fwd != ERR_OK) {
        destroy_matrix(L); destroy_matrix(U); destroy_matrix(y);
        return fwd;
    }

    ErrorCode bwd = backward_substitution(U, y, x);

    destroy_matrix(L); destroy_matrix(U); destroy_matrix(y);
    return bwd;
}

/* === QR-разложение: A = Q * R ===
 * Q — ортогональная матрица (Q^T * Q = I)
 * R — верхняя треугольная матрица
 * Алгоритм: Modified Gram-Schmidt
 */
ErrorCode matrix_qr_decompose(const Matrix* A, Matrix* Q, Matrix* R) {
    if (!A || !Q || !R) return ERR_NULL_POINTER;
    if (A->size != Q->size || A->size != R->size) return ERR_SIZE_MISMATCH;
    if (A->operations != Q->operations || A->operations != R->operations) {
        return ERR_TYPE_MISMATCH;
    }
    
    const AlgebraOperations* ops = A->operations;
    int n = A->size;
    size_t elem_size = A->element_size;
    
    for (int i = 0; i < n * n; i++) {
        ops->zeroFn((char*)R->data + i * elem_size);
    }
    memcpy(Q->data, A->data, n * n * elem_size);
    
    for (int j = 0; j < n; j++) {
        double norm = 0.0;
        for (int i = 0; i < n; i++) {
            void* q_ij = (char*)Q->data + (i * n + j) * elem_size;
            void* prod = malloc(elem_size);
            void* temp = malloc(elem_size);
            ops->multiplyFn(q_ij, q_ij, prod);
            ops->addFn((char*)R->data + (j*n+j)*elem_size, prod, temp);
            memcpy((char*)R->data + (j*n+j)*elem_size, temp, elem_size);
            free(prod); free(temp);
        }
        
        void* norm_elem = malloc(elem_size);
        void* temp = malloc(elem_size);
        ops->sqrtFn((char*)R->data + (j*n+j)*elem_size, norm_elem);
        ops->magnitudeFn(norm_elem, &norm);  
        free(norm_elem);
        
        if (norm < 1e-12) {
            free(temp);
            return ERR_SINGULAR_MATRIX;
        }
        
        ops->setFromDoubleFn((char*)R->data + (j*n+j)*elem_size, norm);
        
        void* norm_val = malloc(elem_size);
        ops->setFromDoubleFn(norm_val, norm);
        
        for (int i = 0; i < n; i++) {
            void* q_ij = (char*)Q->data + (i * n + j) * elem_size;
            ops->divideFn(q_ij, norm_val, temp);
            memcpy(q_ij, temp, elem_size);
        }
        free(norm_val); free(temp);
        
        for (int k = j + 1; k < n; k++) {
            void* dot = malloc(elem_size);
            ops->zeroFn(dot);
            for (int i = 0; i < n; i++) {
                void* q_ij = (char*)Q->data + (i * n + j) * elem_size;
                void* q_ik = (char*)Q->data + (i * n + k) * elem_size;
                void* prod = malloc(elem_size);
                void* tmp = malloc(elem_size);
                ops->multiplyFn(q_ij, q_ik, prod);
                ops->addFn(dot, prod, tmp);
                memcpy(dot, tmp, elem_size);
                free(prod); free(tmp);
            }
            
            memcpy((char*)R->data + (j*n+k)*elem_size, dot, elem_size);
            
            for (int i = 0; i < n; i++) {
                void* q_ik = (char*)Q->data + (i * n + k) * elem_size;
                void* q_ij = (char*)Q->data + (i * n + j) * elem_size;
                void* prod = malloc(elem_size);
                void* tmp = malloc(elem_size);
                ops->multiplyFn(dot, q_ij, prod);
                ops->subtractFn(q_ik, prod, tmp);
                memcpy(q_ik, tmp, elem_size);
                free(prod); free(tmp);
            }
            free(dot);
        }
    }
    return ERR_OK;
}

/* === Решение СЛАУ через QR: A * x = b ===
 * 1. QR-разложение: A = Q * R
 * 2. Q^T * A * x = Q^T * b  →  R * x = Q^T * b
 * 3. Обратная подстановка для R * x = y
 */
ErrorCode solve_qr(const Matrix* A, const Matrix* b, Matrix* x) {
    if (!A || !b || !x) return ERR_NULL_POINTER;
    if (A->size != b->size || A->size != x->size) return ERR_SIZE_MISMATCH;
    if (A->operations != b->operations || A->operations != x->operations) {
        return ERR_TYPE_MISMATCH;
    }

    int n = A->size;
    const AlgebraOperations* ops = A->operations;
    size_t elem_size = A->element_size;

    Matrix* Q = create_matrix(n, ops, elem_size);
    Matrix* R = create_matrix(n, ops, elem_size);
    Matrix* y = create_matrix(n, ops, elem_size);

    if (!Q || !R || !y) {
        destroy_matrix(Q); destroy_matrix(R); destroy_matrix(y);
        return ERR_OUT_OF_MEMORY;
    }

    ErrorCode qr = matrix_qr_decompose(A, Q, R);
    if (qr != ERR_OK) {
        destroy_matrix(Q); destroy_matrix(R); destroy_matrix(y);
        return qr;
    }

    for (int i = 0; i < n; i++) {
        void* y_i = (char*)y->data + i * elem_size;
        ops->zeroFn(y_i);
        for (int j = 0; j < n; j++) {
            void* q_ji = (char*)Q->data + (j * n + i) * elem_size;
            void* b_j = (char*)b->data + j * elem_size;
            void* prod = malloc(elem_size);
            void* temp = malloc(elem_size);
            ops->multiplyFn(q_ji, b_j, prod);
            ops->addFn(y_i, prod, temp);
            memcpy(y_i, temp, elem_size);
            free(prod); free(temp);
        }
    }

    ErrorCode bwd = backward_substitution(R, y, x);

    destroy_matrix(Q); destroy_matrix(R); destroy_matrix(y);
    return bwd;
}

void benchmark_lu_vs_qr(int size) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  BENCHMARK: LU vs QR (%dx%d)          ║\n", size, size);
    printf("╚═══════════════════════════════════════╝\n");

    double* a_vals = malloc(size * size * sizeof(double));
    double* b_vals = malloc(size * sizeof(double));

    if (!a_vals || !b_vals) {
        printf("❌ Ошибка выделения памяти для бенчмарка\n");
        free(a_vals);
        free(b_vals);
        return;
    }

    for (int i = 0; i < size * size; i++) {
        a_vals[i] = (double)(rand() % 90) / 10.0 + 1.0;
    }
    for (int i = 0; i < size; i++) {
        b_vals[i] = (double)(rand() % 90) / 10.0 + 1.0;
    }

    Matrix* A = create_double_matrix(size, a_vals);
    Matrix* b = create_double_matrix(size, b_vals);
    Matrix* x_lu = create_double_matrix(size, NULL);
    Matrix* x_qr = create_double_matrix(size, NULL);

    if (!A || !b || !x_lu || !x_qr) {
        printf("❌ Ошибка создания матриц для бенчмарка\n");
        free(a_vals);
        free(b_vals);
        destroy_matrix(A);
        destroy_matrix(b);
        destroy_matrix(x_lu);
        destroy_matrix(x_qr);
        return;
    }

    const int iterations = 100;
    clock_t lu_start = clock();
    for (int i = 0; i < iterations; i++) {
        solve_lu(A, b, x_lu);
    }
    clock_t lu_end = clock();
    double lu_time =
        (double)(lu_end - lu_start) * 1000.0 / CLOCKS_PER_SEC / iterations;

    clock_t qr_start = clock();
    for (int i = 0; i < iterations; i++) {
        solve_qr(A, b, x_qr);
    }
    clock_t qr_end = clock();
    double qr_time =
        (double)(qr_end - qr_start) * 1000.0 / CLOCKS_PER_SEC / iterations;

    printf("\n┌───────────────────────────────────────┐\n");
    printf("│  Метод      │  Время (мс)  │  Относ.  │\n");
    printf("├───────────────────────────────────────┤\n");
    printf("│  LU         │  %10.3f  │  %5.1fx  │\n", lu_time, 1.0);
    printf("│  QR         │  %10.3f  │  %5.1fx  │\n", qr_time,
           lu_time > 0 ? qr_time / lu_time : 0);
    printf("└───────────────────────────────────────┘\n");

    Double* xlu = (Double*)x_lu->data;
    Double* xqr = (Double*)x_qr->data;
    double max_diff = 0.0;
    for (int i = 0; i < size; i++) {
        double diff = fabs(xlu[i].value - xqr[i].value);
        if (diff > max_diff) max_diff = diff;
    }

    printf("\nМаксимальное расхождение решений: %.2e\n", max_diff);
    printf("Статус: %s\n", (max_diff < 1e-8) ? "✅ Решения совпадают"
                                             : "⚠️  Небольшое расхождение");

    free(a_vals);
    free(b_vals);
    destroy_matrix(A);
    destroy_matrix(b);
    destroy_matrix(x_lu);
    destroy_matrix(x_qr);
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

    if (m->operations != get_double_ops()) {
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

void print_double_vector(const Matrix* v, const char* name) {
    if (!v || !v->data) return;
    if (v->operations != get_double_ops()) {
        printf("Warning: print_double_vector called on non-Double matrix\n");
        return;
    }
    printf("%s:\n", name);
    Double* data = (Double*)v->data;
    for (int i = 0; i < v->size; i++) {
        printf("  x[%d] = %10.4f\n", i, data[i].value);
    }
}