#include "algebra.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

static AlgebraOperations* IntegerOpsInstance = NULL;
static AlgebraOperations* DoubleOpsInstance = NULL;
static AlgebraOperations* ComplexOpsInstance = NULL;

static void integer_zero(void* result) { ((Integer*)result)->value = 0; }

static void integer_one(void* result) { ((Integer*)result)->value = 1; }

static void integer_negate(const void* a, void* result) {
    ((Integer*)result)->value = -((const Integer*)a)->value;
}

static void integer_subtract(const void* a, const void* b, void* result) {
    ((Integer*)result)->value =
        ((const Integer*)a)->value - ((const Integer*)b)->value;
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

static void integer_divide(const void* a, const void* b, void* result) {
    int denom = ((const Integer*)b)->value;
    if (denom == 0) {
        ((Integer*)result)->value = 0; 
    } else {
        ((Integer*)result)->value = ((const Integer*)a)->value / denom;
    }
}


static void integer_sqrt(const void* a, void* result) {
    int val = ((const Integer*)a)->value;
    if (val < 0) {
        ((Integer*)result)->value = 0;  // или обработка ошибки
    } else {
        ((Integer*)result)->value = (int)sqrt((double)val);
    }
}

static void integer_magnitude(const void* a, double* result) {
    *result = (double)abs(((const Integer*)a)->value);
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


static void complex_divide(const void* a, const void* b, void* result) {
    const Complex* z1 = (const Complex*)a;
    const Complex* z2 = (const Complex*)b;
    double denom = (double)z2->re * z2->re + (double)z2->im * z2->im;
    if (denom < 1e-12) {
        ((Complex*)result)->re = 0;
        ((Complex*)result)->im = 0;
    } else {
        ((Complex*)result)->re = (int)((z1->re * z2->re + z1->im * z2->im) / denom);
        ((Complex*)result)->im = (int)((z1->im * z2->re - z1->re * z2->im) / denom);
    }
}

static void complex_sqrt(const void* a, void* result) {
    const Complex* z = (const Complex*)a;
    double r = sqrt((double)z->re * z->re + (double)z->im * z->im);
    double theta = atan2((double)z->im, (double)z->re);
    double sqrt_r = sqrt(r);
    ((Complex*)result)->re = (int)(sqrt_r * cos(theta / 2));
    ((Complex*)result)->im = (int)(sqrt_r * sin(theta / 2));
}

static void complex_magnitude(const void* a, double* result) {
    const Complex* z = (const Complex*)a;
    *result = sqrt((double)z->re * z->re + (double)z->im * z->im);
}

static void double_zero(void* result) { ((Double*)result)->value = 0.0; }

static void double_one(void* result) { ((Double*)result)->value = 1.0; }

static void double_negate(const void* a, void* result) {
    ((Double*)result)->value = -((const Double*)a)->value;
}

static void double_subtract(const void* a, const void* b, void* result) {
    ((Double*)result)->value =
        ((const Double*)a)->value - ((const Double*)b)->value;
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


static void double_divide(const void* a, const void* b, void* result) {
    double denom = ((const Double*)b)->value;
    if (fabs(denom) < 1e-12) {
        ((Double*)result)->value = 0.0;
    } else {
        ((Double*)result)->value = ((const Double*)a)->value / denom;
    }
}

static void double_sqrt(const void* a, void* result) {
    double val = ((const Double*)a)->value;
    ((Double*)result)->value = (val < 0) ? 0.0 : sqrt(val);
}

static void double_magnitude(const void* a, double* result) {
    *result = fabs(((const Double*)a)->value);
}


const AlgebraOperations* GetIntegerOps(void) {
    if (IntegerOpsInstance == NULL) {
        IntegerOpsInstance = malloc(sizeof(AlgebraOperations));
        IntegerOpsInstance->addFn = integer_add;
        IntegerOpsInstance->subtractFn = integer_subtract;
        IntegerOpsInstance->negateFn = integer_negate;
        IntegerOpsInstance->zeroFn = integer_zero;
        IntegerOpsInstance->multiplyFn = integer_multiply;
        IntegerOpsInstance->oneFn = integer_one;
        IntegerOpsInstance->isZeroFn = integer_is_zero;
        IntegerOpsInstance->isOneFn = integer_is_one;
        IntegerOpsInstance->divideFn = integer_divide;
        IntegerOpsInstance->sqrtFn = integer_sqrt;
        IntegerOpsInstance->magnitudeFn = integer_magnitude;
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
        DoubleOpsInstance->divideFn = double_divide;
        DoubleOpsInstance->sqrtFn = double_sqrt;
        DoubleOpsInstance->magnitudeFn = double_magnitude;
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
        ComplexOpsInstance->divideFn = complex_divide;
        ComplexOpsInstance->sqrtFn = complex_sqrt;
        ComplexOpsInstance->magnitudeFn = complex_magnitude;
    }
    return ComplexOpsInstance;
}

