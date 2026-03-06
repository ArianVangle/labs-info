#include "tests.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "algebra.h"
#include "matrix.h"

// Счетчики тестов
static int tests_passed = 0;
static int tests_failed = 0;

// Макрос для проверки
#define TEST_ASSERT(condition, message)         \
    do {                                        \
        if (condition) {                        \
            printf("  ✅ PASS: %s\n", message); \
            tests_passed++;                     \
        } else {                                \
            printf("  ❌ FAIL: %s\n", message); \
            tests_failed++;                     \
        }                                       \
    } while (0)

// tests integer
void test_integer_add(void) {
    printf("\n🔹 ТЕСТ: Integer Matrix Addition\n");
    int a[] = {1, 2, 3, 4};
    int b[] = {5, 6, 7, 8};
    int expected[] = {6, 8, 10, 12};

    Matrix *A = create_integer_matrix(2, a);
    Matrix *B = create_integer_matrix(2, b);
    Matrix *C = create_integer_matrix(2, NULL);
    Matrix *Expected = create_integer_matrix(2, expected);

    matrix_add(A, B, C);
    TEST_ASSERT(integer_matrices_equal(C, Expected), "A + B = Expected");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
    destroy_matrix(Expected);
}

void test_integer_multiply(void) {
    printf("\n🔹 ТЕСТ: Integer Matrix Multiplication\n");
    int a[] = {1, 2, 3, 4};
    int b[] = {5, 6, 7, 8};
    int expected[] = {19, 22, 43, 50};

    Matrix *A = create_integer_matrix(2, a);
    Matrix *B = create_integer_matrix(2, b);
    Matrix *C = create_integer_matrix(2, NULL);
    Matrix *Expected = create_integer_matrix(2, expected);

    matrix_multiply(A, B, C);
    TEST_ASSERT(integer_matrices_equal(C, Expected), "A × B = Expected");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
    destroy_matrix(Expected);
}

void test_integer_scalar(void) {
    printf("\n🔹 ТЕСТ: Integer Matrix × Scalar\n");
    int a[] = {1, 2, 3, 4};
    int expected[] = {3, 6, 9, 12};

    Matrix *A = create_integer_matrix(2, a);
    Matrix *C = create_integer_matrix(2, NULL);
    Integer scalar = {.value = 3};

    matrix_multiply_scalar(A, &scalar, C);
    Matrix *Expected = create_integer_matrix(2, expected);
    TEST_ASSERT(integer_matrices_equal(C, Expected), "A × 3 = Expected");

    destroy_matrix(A);
    destroy_matrix(C);
    destroy_matrix(Expected);
}

void test_integer_operations(void) {
    test_integer_add();
    test_integer_multiply();
    test_integer_scalar();
}

// tests complex
void test_complex_add(void) {
    printf("\n🔹 ТЕСТ: Complex Matrix Addition\n");
    Matrix *A =
        create_complex_matrix(2, (int[]){1, 3, 5, 7}, (int[]){2, 4, 6, 8});
    Matrix *B =
        create_complex_matrix(2, (int[]){2, 1, 3, 4}, (int[]){1, 2, 3, 4});
    Matrix *C = create_complex_matrix(2, NULL, NULL);
    Matrix *Expected =
        create_complex_matrix(2, (int[]){3, 4, 8, 11}, (int[]){3, 6, 9, 12});

    matrix_add(A, B, C);
    TEST_ASSERT(complex_matrices_equal(C, Expected),
                "Complex A + B = Expected");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
    destroy_matrix(Expected);
}

void test_complex_multiply(void) {
    printf("\n🔹 ТЕСТ: Complex Matrix Multiplication\n");
    Matrix *A =
        create_complex_matrix(2, (int[]){1, 0, 1, 2}, (int[]){0, 1, 1, 0});
    Matrix *B =
        create_complex_matrix(2, (int[]){1, 1, 0, 1}, (int[]){0, 0, 1, 1});
    Matrix *C = create_complex_matrix(2, NULL, NULL);
    Matrix *Expected =
        create_complex_matrix(2, (int[]){0, 0, 1, 3}, (int[]){0, 1, 3, 3});

    matrix_multiply(A, B, C);
    TEST_ASSERT(complex_matrices_equal(C, Expected),
                "Complex A × B = Expected");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
    destroy_matrix(Expected);
}

void test_complex_operations(void) {
    test_complex_add();
    test_complex_multiply();
    test_complex_multiply_scalar();
    test_complex_multiply_real_scalar();
    test_complex_multiply_imaginary_unit();
}

void test_complex_multiply_scalar(void) {
    printf("\n🔹 ТЕСТ: Complex Matrix × Scalar\n");
    printf("==================================\n");

    int a_re[] = {1, 3, 5, 7};
    int a_im[] = {2, 4, 6, 8};
    int exp_re[] = {-1, -1, -1, -1};
    int exp_im[] = {3, 7, 11, 15};

    Matrix *A = create_complex_matrix(2, a_re, a_im);
    Matrix *C = create_complex_matrix(2, NULL, NULL);

    Complex scalar = {.re = 1, .im = 1};

    printf("Matrix A:\n");
    print_complex_matrix(A, "A");
    printf("Scalar: %d+%di\n", scalar.re, scalar.im);

    matrix_multiply_scalar(A, &scalar, C);

    printf("Result A × scalar:\n");
    print_complex_matrix(C, "A × scalar");

    Matrix *Expected = create_complex_matrix(2, exp_re, exp_im);

    if (complex_matrices_equal(C, Expected)) {
        printf("✅ PASS: Результат совпадает с ожидаемым\n");
        tests_passed++;
    } else {
        printf("❌ FAIL: Результат НЕ совпадает!\n");
        printf("Expected:\n");
        print_complex_matrix(Expected, "Expected");
        tests_failed++;
    }

    destroy_matrix(A);
    destroy_matrix(C);
    destroy_matrix(Expected);
}

//  Умножение на вещественный скаляр (im = 0)
void test_complex_multiply_real_scalar(void) {
    printf("\n🔹 ТЕСТ: Complex Matrix × Real Scalar (im=0)\n");
    printf("==============================================\n");

    int a_re[] = {1, 3, 5, 7};
    int a_im[] = {2, 4, 6, 8};
    int exp_re[] = {2, 6, 10, 14};
    int exp_im[] = {4, 8, 12, 16};

    Matrix *A = create_complex_matrix(2, a_re, a_im);
    Matrix *C = create_complex_matrix(2, NULL, NULL);

    Complex scalar = {.re = 2, .im = 0};

    printf("Matrix A:\n");
    print_complex_matrix(A, "A");
    printf("Scalar: %d+%di\n", scalar.re, scalar.im);

    matrix_multiply_scalar(A, &scalar, C);

    printf("Result A × scalar:\n");
    print_complex_matrix(C, "A × scalar");

    Matrix *Expected = create_complex_matrix(2, exp_re, exp_im);

    if (complex_matrices_equal(C, Expected)) {
        printf("✅ PASS: Результат совпадает с ожидаемым\n");
        tests_passed++;
    } else {
        printf("❌ FAIL: Результат НЕ совпадает!\n");
        tests_failed++;
    }

    destroy_matrix(A);
    destroy_matrix(C);
    destroy_matrix(Expected);
}

//  Умножение на мнимую единицу (0+1i)
void test_complex_multiply_imaginary_unit(void) {
    printf("\n🔹 ТЕСТ: Complex Matrix × Imaginary Unit (0+1i)\n");
    printf("================================================\n");

    int a_re[] = {1, 0, 1, 2};
    int a_im[] = {0, 1, 1, 0};
    int exp_re[] = {0, -1, -1, 0};
    int exp_im[] = {1, 0, 1, 2};

    Matrix *A = create_complex_matrix(2, a_re, a_im);
    Matrix *C = create_complex_matrix(2, NULL, NULL);

    Complex scalar = {.re = 0, .im = 1};

    printf("Matrix A:\n");
    print_complex_matrix(A, "A");
    printf("Scalar: %d+%di (imaginary unit i)\n", scalar.re, scalar.im);

    matrix_multiply_scalar(A, &scalar, C);

    printf("Result A × i:\n");
    print_complex_matrix(C, "A × i");

    Matrix *Expected = create_complex_matrix(2, exp_re, exp_im);

    if (complex_matrices_equal(C, Expected)) {
        printf("✅ PASS: Результат совпадает с ожидаемым\n");
        tests_passed++;
    } else {
        printf("❌ FAIL: Результат НЕ совпадает!\n");
        tests_failed++;
    }

    destroy_matrix(A);
    destroy_matrix(C);
    destroy_matrix(Expected);
}

// edge cases
void test_edge_cases(void) {
    printf("\n🔹 ТЕСТ: Edge Cases\n");

    // 1×1 матрица
    Matrix *A1 = create_integer_matrix(1, (int[]){5});
    Matrix *B1 = create_integer_matrix(1, (int[]){7});
    Matrix *C1 = create_integer_matrix(1, NULL);
    matrix_multiply(A1, B1, C1);
    TEST_ASSERT(((Integer *)C1->data)[0].value == 35, "1×1 multiply: 5×7=35");
    destroy_matrix(A1);
    destroy_matrix(B1);
    destroy_matrix(C1);

    // Нулевая матрица
    Matrix *Zero = create_integer_matrix(2, (int[]){0, 0, 0, 0});
    Matrix *A = create_integer_matrix(2, (int[]){1, 2, 3, 4});
    Matrix *C = create_integer_matrix(2, NULL);
    matrix_add(A, Zero, C);
    TEST_ASSERT(integer_matrices_equal(A, C), "A + 0 = A");
    destroy_matrix(Zero);
    destroy_matrix(A);
    destroy_matrix(C);

    // Отрицательные числа
    Matrix *NegA = create_integer_matrix(2, (int[]){-1, 2, -3, 4});
    Matrix *NegB = create_integer_matrix(2, (int[]){1, -2, 3, -4});
    Matrix *NegC = create_integer_matrix(2, NULL);
    matrix_add(NegA, NegB, NegC);
    Integer *data = (Integer *)NegC->data;
    TEST_ASSERT(data[0].value == 0 && data[1].value == 0,
                "Negative numbers addition");
    destroy_matrix(NegA);
    destroy_matrix(NegB);
    destroy_matrix(NegC);
}

// type tests
void test_type_safety(void) {
    printf("\n🔹 ТЕСТ: Type Safety (no mixing types)\n");

    Matrix *IntM = create_integer_matrix(2, (int[]){1, 2, 3, 4});
    Matrix *CompM =
        create_complex_matrix(2, (int[]){1, 2, 3, 4}, (int[]){0, 0, 0, 0});
    Matrix *Result = create_integer_matrix(2, NULL);

    printf("  Attempting Integer + Complex (should fail safely)...\n");
    matrix_add(IntM, CompM, Result);
    TEST_ASSERT(IntM->operations != CompM->operations,
                "Type pointers are different");

    destroy_matrix(IntM);
    destroy_matrix(CompM);
    destroy_matrix(Result);
}

void test_lu_double_simple(void) {
    printf("\n🔹 ТЕСТ: LU Decomposition (Double, 2x2)\n");

    double a_vals[] = {4, 3, 6, 3};
    Matrix *A = create_double_matrix(2, a_vals);
    Matrix *L = create_double_matrix(2, NULL);
    Matrix *U = create_double_matrix(2, NULL);

    clock_t start = clock();
    int result = matrix_lu_decompose(A, L, U);
    clock_t end = clock();
    double time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("  ⏱  Время выполнения: %.3f мс\n", time_ms);
    TEST_ASSERT(result == 0, "LU decomposition succeeded");

    Double *l_data = (Double *)L->data;
    Double *u_data = (Double *)U->data;

    TEST_ASSERT(fabs(l_data[0].value - 1.0) < 1e-10, "L[0][0] == 1.0");
    TEST_ASSERT(fabs(u_data[0].value - 4.0) < 1e-10, "U[0][0] == 4.0");

    Matrix *LU = create_double_matrix(2, NULL);
    matrix_multiply(L, U, LU);
    TEST_ASSERT(double_matrices_equal(LU, A, 1e-10), "L * U == A");

    destroy_matrix(A);
    destroy_matrix(L);
    destroy_matrix(U);
    destroy_matrix(LU);
}

void test_lu_double_identity(void) {
    printf("\n🔹 ТЕСТ: LU Decomposition (Identity matrix)\n");

    double a_vals[] = {1, 0, 0, 1};
    Matrix *A = create_double_matrix(2, a_vals);
    Matrix *L = create_double_matrix(2, NULL);
    Matrix *U = create_double_matrix(2, NULL);

    clock_t start = clock();
    int result = matrix_lu_decompose(A, L, U);
    clock_t end = clock();

    double time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("  ⏱  Время выполнения: %.3f мс\n", time_ms);
    TEST_ASSERT(result == 0, "LU of identity succeeded");

    Double *l_data = (Double *)L->data;
    Double *u_data = (Double *)U->data;

    TEST_ASSERT(fabs(l_data[0].value - 1.0) < 1e-10 &&
                    fabs(l_data[3].value - 1.0) < 1e-10,
                "L is identity");
    TEST_ASSERT(fabs(u_data[0].value - 1.0) < 1e-10 &&
                    fabs(u_data[3].value - 1.0) < 1e-10,
                "U is identity");

    destroy_matrix(A);
    destroy_matrix(L);
    destroy_matrix(U);
}

void test_lu_double_singular(void) {
    printf("\n🔹 ТЕСТ: LU Decomposition (Singular matrix, should fail)\n");

    double a_vals[] = {1, 2, 2, 4};
    Matrix *A = create_double_matrix(2, a_vals);
    Matrix *L = create_double_matrix(2, NULL);
    Matrix *U = create_double_matrix(2, NULL);

    clock_t start = clock();
    int result = matrix_lu_decompose(A, L, U);
    clock_t end = clock();

    double time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("  ⏱  Время выполнения: %.3f мс\n", time_ms);
    TEST_ASSERT(result == -2, "LU fails on singular matrix");

    destroy_matrix(A);
    destroy_matrix(L);
    destroy_matrix(U);
}

void test_lu_integer_to_double(void) {
    printf("\n🔹 ТЕСТ: LU Integer → Double result\n");

    int a_vals[] = {2, 1, 1, 2};
    Matrix *A = create_integer_matrix(2, a_vals);

    Matrix *L = create_double_matrix(2, NULL);
    Matrix *U = create_double_matrix(2, NULL);

    clock_t start = clock();
    int result = matrix_lu_decompose(A, L, U);
    clock_t end = clock();

    double time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("  ⏱  Время выполнения: %.3f мс\n", time_ms);
    TEST_ASSERT(result == 0, "Integer→Double LU succeeded");

    Double *l_data = (Double *)L->data;
    TEST_ASSERT(fabs(l_data[2].value - 0.5) < 1e-10, "L[1][0] == 0.5");

    destroy_matrix(A);
    destroy_matrix(L);
    destroy_matrix(U);
}

void test_lu_operations(void) {
    test_lu_double_simple();
    test_lu_double_identity();
    test_lu_double_singular();
    test_lu_integer_to_double();
}

// run all tests
void run_all_tests(void) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║  POLYMORPHIC MATRIX TEST SUITE v1.1    ║\n");
    printf("║  + LU Decomposition support            ║\n");
    printf("╚════════════════════════════════════════╝\n");

    test_integer_operations();
    test_complex_operations();
    test_edge_cases();
    test_type_safety();
    test_lu_operations();

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  RESULTS: %d passed, %d failed         ║\n", tests_passed,
           tests_failed);
    printf("╚═══════════════════════════════════════╝\n");

    if (tests_failed > 0) {
        exit(1);
    }
}