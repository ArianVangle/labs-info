#include "tests.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Complex Matrix × Scalar

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

    // Попытка сложить Integer + Complex должна быть отклонена
    printf("  Attempting Integer + Complex (should fail safely)...\n");
    matrix_add(IntM, CompM, Result);  // Должно вывести ошибку в stderr
    TEST_ASSERT(IntM->operations != CompM->operations,
                "Type pointers are different");

    destroy_matrix(IntM);
    destroy_matrix(CompM);
    destroy_matrix(Result);
}

// run all tests
void run_all_tests(void) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║  POLYMORPHIC MATRIX TEST SUITE v1.0    ║\n");
    printf("║  Variant 21: Integer & Complex         ║\n");
    printf("╚════════════════════════════════════════╝\n");

    test_integer_operations();
    test_complex_operations();
    test_edge_cases();
    test_type_safety();

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  RESULTS: %d passed, %d failed          ║\n", tests_passed,
           tests_failed);
    printf("╚═══════════════════════════════════════╝\n");

    if (tests_failed > 0) {
        exit(1);
    }
}