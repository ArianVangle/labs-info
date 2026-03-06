#include "algebra.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


static AlgebraOperations* IntegerOpsInstance = NULL;
static AlgebraOperations* DoubleOpsInstance = NULL;
static AlgebraOperations* ComplexOpsInstance = NULL;


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
                Complex prod = {
                    l_val.re * u_val.re - l_val.im * u_val.im,
                    l_val.re * u_val.im + l_val.im * u_val.re
                };
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
                Complex prod = {
                    l_val.re * u_val.re - l_val.im * u_val.im,
                    l_val.re * u_val.im + l_val.im * u_val.re
                };
                sum.re += prod.re;
                sum.im += prod.im;
            }
            Complex numerator = {
                a[i * n + k].re - sum.re,
                a[i * n + k].im - sum.im
            };
            Complex u_kk = u[k * n + k];
            
            if (complex_magnitude(&u_kk) < 1e-12) return -2;
            
            Complex result = complex_div_helper(&numerator, &u_kk);
            l[i * n + k] = result;
        }
    }
    return 0;
}

const AlgebraOperations* GetIntegerOps(void) {
    if (IntegerOpsInstance == NULL) {
        IntegerOpsInstance = malloc(sizeof(AlgebraOperations));
        if (IntegerOpsInstance) {
            IntegerOpsInstance->addFn = integer_add;
            IntegerOpsInstance->multiplyFn = integer_multiply;
            IntegerOpsInstance->lu_decompose_fn = integer_lu_decompose;  
        }
    }
    return IntegerOpsInstance;
}

const AlgebraOperations* GetDoubleOps(void) {
    if (DoubleOpsInstance == NULL) {
        DoubleOpsInstance = malloc(sizeof(AlgebraOperations));
        if (DoubleOpsInstance) {
            DoubleOpsInstance->addFn = double_add;
            DoubleOpsInstance->multiplyFn = double_multiply;
            DoubleOpsInstance->lu_decompose_fn = double_lu_decompose;
        }
    }
    return DoubleOpsInstance;
}

const AlgebraOperations* GetComplexOps(void) {
    if (ComplexOpsInstance == NULL) {
        ComplexOpsInstance = malloc(sizeof(AlgebraOperations));
        if (ComplexOpsInstance) {
            ComplexOpsInstance->addFn = complex_add;
            ComplexOpsInstance->multiplyFn = complex_multiply;
            ComplexOpsInstance->lu_decompose_fn = complex_lu_decompose; 
        }
    }
    return ComplexOpsInstance;
}