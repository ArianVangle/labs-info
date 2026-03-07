#include "algebra.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static AlgebraOperations* IntegerOpsInstance = NULL;
static AlgebraOperations* DoubleOpsInstance = NULL;
static AlgebraOperations* ComplexOpsInstance = NULL;


static void integer_zero(void* result) {
    ((Integer*)result)->value = 0;
}

static void integer_one(void* result) {
    ((Integer*)result)->value = 1;
}

static void integer_negate(const void* a, void* result) {
    ((Integer*)result)->value = -((const Integer*)a)->value;
}

static void integer_subtract(const void* a, const void* b, void* result) {
    ((Integer*)result)->value = ((const Integer*)a)->value - ((const Integer*)b)->value;
}

static int integer_is_zero(const void* a) {
    return ((const Integer*)a)->value == 0;
}

static int integer_is_one(const void* a) {
    return ((const Integer*)a)->value == 1;
}

static void integer_add(const void* e1, const void* e2, void* result) {
    ((Integer*)result)->value =
        ((const Integer*)e1)->value + ((const Integer*)e2)->value;
}

static void integer_multiply(const void* e1, const void* e2, void* result) {
    ((Integer*)result)->value =
        ((const Integer*)e1)->value * ((const Integer*)e2)->value;
}

static void complex_zero(void* result) {
    ((Complex*)result)->re = 0;
    ((Complex*)result)->im = 0;
}

static void complex_one(void* result) {
    ((Complex*)result)->re = 1;
    ((Complex*)result)->im = 0;
}

static void complex_negate(const void* a, void* result) {
    ((Complex*)result)->re = -((const Complex*)a)->re;
    ((Complex*)result)->im = -((const Complex*)a)->im;
}

static void complex_subtract(const void* a, const void* b, void* result) {
    ((Complex*)result)->re = ((const Complex*)a)->re - ((const Complex*)b)->re;
    ((Complex*)result)->im = ((const Complex*)a)->im - ((const Complex*)b)->im;
}

static int complex_is_zero(const void* a) {
    const Complex* c = (const Complex*)a;
    return (fabs((double)c->re) < 1e-12) && (fabs((double)c->im) < 1e-12); 
}

static int complex_is_one(const void* a) {
    const Complex* c = (const Complex*)a;
    return (fabs((double)c->re - 1.0) < 1e-12) && (fabs((double)c->im) < 1e-12);  
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
static int complex_equal(const Complex* a, const Complex* b, double epsilon) {
    return (fabs((double)a->re - (double)b->re) < epsilon) &&
           (fabs((double)a->im - (double)b->im) < epsilon);
}

static void double_zero(void* result) {
    ((Double*)result)->value = 0.0;
}

static void double_one(void* result) {
    ((Double*)result)->value = 1.0;
}

static void double_negate(const void* a, void* result) {
    ((Double*)result)->value = -((const Double*)a)->value;
}

static void double_subtract(const void* a, const void* b, void* result) {
    ((Double*)result)->value = ((const Double*)a)->value - ((const Double*)b)->value;
}

static int double_is_zero(const void* a) {
    return fabs(((const Double*)a)->value) < 1e-12;
}

static int double_is_one(const void* a) {
    return fabs(((const Double*)a)->value - 1.0) < 1e-12;
}


static void double_add(const void* e1, const void* e2, void* result) {
    ((Double*)result)->value =
        ((const Double*)e1)->value + ((const Double*)e2)->value;
}

static void double_multiply(const void* e1, const void* e2, void* result) {
    ((Double*)result)->value =
        ((const Double*)e1)->value * ((const Double*)e2)->value;
}

static Complex complex_div_helper(const Complex* a, const Complex* b) {
    double denom = (double)b->re * b->re + (double)b->im * b->im;
    if (denom < 1e-12) return (Complex){0, 0};

    double re = ((double)a->re * b->re + (double)a->im * b->im) / denom;
    double im = ((double)a->im * b->re - (double)a->re * b->im) / denom;
    return (Complex){(int)re, (int)im};
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

static double complex_magnitude(const Complex* c) {
    return sqrt((double)c->re * c->re + (double)c->im * c->im);
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

static int integer_lu_decompose(const Matrix* A, Matrix* L, Matrix* U) {
    if (L->operations != GetDoubleOps() || U->operations != GetDoubleOps()) {
        return -3;  // Тип результата должен быть Double
    }

    int n = A->size;
    Integer* a = (Integer*)A->data;
    Double* l = (Double*)L->data;
    Double* u = (Double*)U->data;

    for (int i = 0; i < n * n; i++) {
        l[i].value = 0.0;
        u[i].value = 0.0;
    }
    for (int i = 0; i < n; i++) {
        l[i * n + i].value = 1.0;
    }

    for (int k = 0; k < n; k++) {
        for (int j = k; j < n; j++) {
            double sum = 0.0;
            for (int m = 0; m < k; m++) {
                sum += l[k * n + m].value * u[m * n + j].value;
            }
            u[k * n + j].value = (double)a[k * n + j].value - sum;
        }
        for (int i = k + 1; i < n; i++) {
            double sum = 0.0;
            for (int m = 0; m < k; m++) {
                sum += l[i * n + m].value * u[m * n + k].value;
            }
            double u_kk = u[k * n + k].value;
            if (fabs(u_kk) < 1e-12) return -2;
            l[i * n + k].value = ((double)a[i * n + k].value - sum) / u_kk;
        }
    }
    return 0;
}

static int double_lu_decompose(const Matrix* A, Matrix* L, Matrix* U) {
    if (!A || !L || !U) return -1;
    if (A->size != L->size || A->size != U->size) return -1;

    int n = A->size;
    Double* a = (Double*)A->data;
    Double* l = (Double*)L->data;
    Double* u = (Double*)U->data;

    for (int i = 0; i < n * n; i++) {
        l[i].value = 0.0;
        u[i].value = 0.0;
    }
    for (int i = 0; i < n; i++) {
        l[i * n + i].value = 1.0;
    }

    for (int k = 0; k < n; k++) {
        for (int j = k; j < n; j++) {
            double sum = 0.0;
            for (int m = 0; m < k; m++) {
                sum += l[k * n + m].value * u[m * n + j].value;
            }
            u[k * n + j].value = a[k * n + j].value - sum;

            if (j == k && fabs(u[k * n + k].value) < 1e-12) {
                return -2;  // Вырожденная матрица
            }
        }

        for (int i = k + 1; i < n; i++) {
            double sum = 0.0;
            for (int m = 0; m < k; m++) {
                sum += l[i * n + m].value * u[m * n + k].value;
            }
            double u_kk = u[k * n + k].value;
            if (fabs(u_kk) < 1e-12) return -2;  // Вырожденная матрица
            l[i * n + k].value = (a[i * n + k].value - sum) / u_kk;
        }
    }
    return 0;
}

static int complex_lu_decompose(const Matrix* A, Matrix* L, Matrix* U) {
    if (!A || !L || !U) return -1;
    if (A->size != L->size || A->size != U->size) return -1;

    int n = A->size;
    Complex* a = (Complex*)A->data;
    Complex* l = (Complex*)L->data;
    Complex* u = (Complex*)U->data;

    for (int i = 0; i < n * n; i++) {
        l[i].re = l[i].im = 0;
        u[i].re = u[i].im = 0;
    }
    for (int i = 0; i < n; i++) {
        l[i * n + i].re = 1;
        l[i * n + i].im = 0;
    }

    for (int k = 0; k < n; k++) {
        for (int j = k; j < n; j++) {
            Complex sum = {0, 0};
            for (int m = 0; m < k; m++) {
                Complex l_val = l[k * n + m];
                Complex u_val = u[m * n + j];
                Complex prod = {l_val.re * u_val.re - l_val.im * u_val.im,
                                l_val.re * u_val.im + l_val.im * u_val.re};
                sum.re += prod.re;
                sum.im += prod.im;
            }
            u[k * n + j].re = a[k * n + j].re - sum.re;
            u[k * n + j].im = a[k * n + j].im - sum.im;
        }

        for (int i = k + 1; i < n; i++) {
            Complex sum = {0, 0};
            for (int m = 0; m < k; m++) {
                Complex l_val = l[i * n + m];
                Complex u_val = u[m * n + k];
                Complex prod = {l_val.re * u_val.re - l_val.im * u_val.im,
                                l_val.re * u_val.im + l_val.im * u_val.re};
                sum.re += prod.re;
                sum.im += prod.im;
            }
            Complex numerator = {a[i * n + k].re - sum.re,
                                 a[i * n + k].im - sum.im};
            Complex u_kk = u[k * n + k];

            if (complex_magnitude(&u_kk) < 1e-12) return -2;

            Complex result = complex_div_helper(&numerator, &u_kk);
            l[i * n + k] = result;
        }
    }
    return 0;
}

const AlgebraOperations* GetIntegerOps(void) {
    if (IntegerOpsInstance== NULL) {
        IntegerOpsInstance = malloc(sizeof(AlgebraOperations));
        IntegerOpsInstance->addFn = integer_add;
        IntegerOpsInstance->subtractFn = integer_subtract;
        IntegerOpsInstance->negateFn = integer_negate;
        IntegerOpsInstance->zeroFn = integer_zero;
        IntegerOpsInstance->multiplyFn = integer_multiply;
        IntegerOpsInstance->oneFn = integer_one;
        IntegerOpsInstance->isZeroFn = integer_is_zero;
        IntegerOpsInstance->isOneFn = integer_is_one;
        IntegerOpsInstance->lu_decompose_fn = integer_lu_decompose;
    }
    return IntegerOpsInstance;
}

const AlgebraOperations* GetDoubleOps(void) {
    if (DoubleOpsInstance == NULL) {
        DoubleOpsInstance = malloc(sizeof(AlgebraOperations));
        DoubleOpsInstance->addFn = double_add;
        DoubleOpsInstance->subtractFn = double_subtract;
        DoubleOpsInstance->negateFn = double_negate;
        DoubleOpsInstance->zeroFn = double_zero;
        DoubleOpsInstance->multiplyFn = double_multiply;
        DoubleOpsInstance->oneFn = double_one;
        DoubleOpsInstance->isZeroFn = double_is_zero;
        DoubleOpsInstance->isOneFn = double_is_one;
        DoubleOpsInstance->lu_decompose_fn = double_lu_decompose;
    }
    return DoubleOpsInstance;
}

const AlgebraOperations* GetComplexOps(void) {
    if (ComplexOpsInstance == NULL) {
        ComplexOpsInstance = malloc(sizeof(AlgebraOperations));
        ComplexOpsInstance->addFn = complex_add;
        ComplexOpsInstance->subtractFn = complex_subtract;
        ComplexOpsInstance->negateFn = complex_negate;
        ComplexOpsInstance->zeroFn = complex_zero;
        ComplexOpsInstance->multiplyFn = complex_multiply;
        ComplexOpsInstance->oneFn = complex_one;
        ComplexOpsInstance->isZeroFn = complex_is_zero;
        ComplexOpsInstance->isOneFn = complex_is_one;
        ComplexOpsInstance->lu_decompose_fn = complex_lu_decompose;
    }
    return ComplexOpsInstance;
}


int test_ring_axioms(const AlgebraOperations* ops) {
    if (!ops) return -1;
    
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  RING AXIOMS VERIFICATION             ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    
    size_t elem_size = (ops == GetDoubleOps()) ? sizeof(Double) :
                       (ops == GetComplexOps()) ? sizeof(Complex) : sizeof(Integer);
    
    void* a = malloc(elem_size);
    void* b = malloc(elem_size);
    void* c = malloc(elem_size);
    void* temp1 = malloc(elem_size);
    void* temp2 = malloc(elem_size);
    
    int passed = 0;
    int total = 7;
    
    // Инициализация тестовых значений
    if (ops == GetDoubleOps()) {
        ((Double*)a)->value = 2.0;
        ((Double*)b)->value = 3.0;
        ((Double*)c)->value = 4.0;
    } else if (ops == GetIntegerOps()) {
        ((Integer*)a)->value = 2;
        ((Integer*)b)->value = 3;
        ((Integer*)c)->value = 4;
    } else if (ops == GetComplexOps()) {
        ((Complex*)a)->re = 2; ((Complex*)a)->im = 1;
        ((Complex*)b)->re = 3; ((Complex*)b)->im = 2;
        ((Complex*)c)->re = 4; ((Complex*)c)->im = 1;
    }
    
    // Замкнутость сложения
    ops->zeroFn(a);
    ops->zeroFn(b);
    ops->addFn(a, b, c);
    if (ops->isZeroFn(c)) {
        printf("  ✅ 1. Замкнутость сложения\n");
        passed++;
    } else {
        printf("  ❌ 1. Замкнутость сложения\n");
    }
    
    // Ассоциативность
    ops->addFn(a, b, temp1);
    ops->addFn(temp1, c, temp1);
    ops->addFn(b, c, temp2);
    ops->addFn(a, temp2, temp2);
    
    int assoc_ok = 0;
    if (ops == GetDoubleOps()) {
        assoc_ok = (fabs(((Double*)temp1)->value - ((Double*)temp2)->value) < 1e-12);
    } else if (ops == GetIntegerOps()) {
        assoc_ok = (((Integer*)temp1)->value == ((Integer*)temp2)->value);
    } else if (ops == GetComplexOps()) {
        assoc_ok = complex_equal((Complex*)temp1, (Complex*)temp2, 1e-12);
    }
    if (assoc_ok) { printf("  ✅ 2. Ассоциативность сложения\n"); passed++; }
    else { printf("  ❌ 2. Ассоциативность сложения\n"); }
    
    // Коммутативность
    ops->addFn(a, b, temp1);
    ops->addFn(b, a, temp2);
    
    int comm_ok = 0;
    if (ops == GetDoubleOps()) {
        comm_ok = (fabs(((Double*)temp1)->value - ((Double*)temp2)->value) < 1e-12);
    } else if (ops == GetIntegerOps()) {
        comm_ok = (((Integer*)temp1)->value == ((Integer*)temp2)->value);
    } else if (ops == GetComplexOps()) {
        comm_ok = complex_equal((Complex*)temp1, (Complex*)temp2, 1e-12);
    }
    if (comm_ok) { printf("  ✅ 3. Коммутативность сложения\n"); passed++; }
    else { printf("  ❌ 3. Коммутативность сложения\n"); }
    
    // Нейтральный элемент сложения 
    ops->zeroFn(temp1);
    ops->addFn(a, temp1, temp2);
    
    int zero_ok = 0;
    if (ops == GetDoubleOps()) {
        zero_ok = (fabs(((Double*)a)->value - ((Double*)temp2)->value) < 1e-12);
    } else if (ops == GetIntegerOps()) {
        zero_ok = (((Integer*)a)->value == ((Integer*)temp2)->value);
    } else if (ops == GetComplexOps()) {
        zero_ok = complex_equal((Complex*)a, (Complex*)temp2, 1e-12);
    }
    if (zero_ok) { printf("  ✅ 4. Нейтральный элемент сложения (0)\n"); passed++; }
    else { printf("  ❌ 4. Нейтральный элемент сложения (0)\n"); }
    
    // Аддитивный обратный
    ops->negateFn(a, temp1);
    ops->addFn(a, temp1, temp2);
    if (ops->isZeroFn(temp2)) {
        printf("  ✅ 5. Аддитивный обратный (-a)\n");
        passed++;
    } else {
        printf("  ❌ 5. Аддитивный обратный (-a)\n");
    }
    
    // Нейтральный элемент умножения
    ops->oneFn(temp1);
    ops->multiplyFn(a, temp1, temp2);
    
    int one_ok = 0;
    if (ops == GetDoubleOps()) {
        one_ok = (fabs(((Double*)a)->value - ((Double*)temp2)->value) < 1e-12);
    } else if (ops == GetIntegerOps()) {
        one_ok = (((Integer*)a)->value == ((Integer*)temp2)->value);
    } else if (ops == GetComplexOps()) {
        one_ok = complex_equal((Complex*)a, (Complex*)temp2, 1e-12);
    }
    if (one_ok) { printf("  ✅ 6. Нейтральный элемент умножения (1)\n"); passed++; }
    else { printf("  ❌ 6. Нейтральный элемент умножения (1)\n"); }
    
    // Дистрибутивность
    ops->addFn(b, c, temp1);        
    ops->multiplyFn(a, temp1, temp1); 
    ops->multiplyFn(a, b, temp2);   
    ops->multiplyFn(a, c, c);       
    ops->addFn(temp2, c, temp2);    
    
    int dist_ok = 0;
    if (ops == GetDoubleOps()) {
        dist_ok = (fabs(((Double*)temp1)->value - ((Double*)temp2)->value) < 1e-12);
    } else if (ops == GetIntegerOps()) {
        dist_ok = (((Integer*)temp1)->value == ((Integer*)temp2)->value);
    } else if (ops == GetComplexOps()) {
        dist_ok = complex_equal((Complex*)temp1, (Complex*)temp2, 1e-12);
    }
    if (dist_ok) { printf("  ✅ 7. Дистрибутивность\n"); passed++; }
    else { printf("  ❌ 7. Дистрибутивность\n"); }
    
    printf("\n┌───────────────────────────────────────┐\n");
    printf("│  Результат: %d/%d аксиом выполнено      │\n", passed, total);
    printf("└───────────────────────────────────────┘\n");
    
    free(a); free(b); free(c); free(temp1); free(temp2);
    
    return (passed == total) ? 0 : -1;
}