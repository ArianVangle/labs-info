#include "interface.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tests.h"

static void print_error(const char* context, ErrorCode code) {
    if (code != ERR_OK) {
        printf("❌ %s: %s (код: %d)\n", context, error_message(code), code);
    }
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

Matrix* input_integer_matrix(int size) {
    Matrix* m = create_integer_matrix(size, NULL);
    if (!m) return NULL;

    Integer* data = (Integer*)m->data;
    printf("Введите %d элементов матрицы %dx%d (целые числа):\n", size * size,
           size, size);

    for (int i = 0; i < size * size; i++) {
        int row = i / size;
        int col = i % size;
        printf("  [%d][%d] = ", row, col);
        if (scanf("%d", &data[i].value) != 1) {
            printf("❌ Ошибка ввода!\n");
            clear_input_buffer();
            destroy_matrix(m);
            return NULL;
        }
    }

    return m;
}

Matrix* input_complex_matrix(int size) {
    Matrix* m = create_complex_matrix(size, NULL, NULL);
    if (!m) return NULL;

    Complex* data = (Complex*)m->data;
    printf("Введите %d элементов матрицы %dx%d (комплексные числа):\n",
           size * size, size, size);

    for (int i = 0; i < size * size; i++) {
        int row = i / size;
        int col = i % size;
        printf("  [%d][%d] (re im) = ", row, col);
        if (scanf("%d %d", &data[i].re, &data[i].im) != 2) {
            printf("❌ Ошибка ввода!\n");
            clear_input_buffer();
            destroy_matrix(m);
            return NULL;
        }
    }

    return m;
}

Matrix* input_double_matrix(int size) {
    Matrix* m = create_double_matrix(size, NULL);
    if (!m) return NULL;

    Double* data = (Double*)m->data;
    printf("Введите %d элементов матрицы %dx%d (вещественные числа):\n",
           size * size, size, size);

    for (int i = 0; i < size * size; i++) {
        int row = i / size;
        int col = i % size;
        printf("  [%d][%d] = ", row, col);
        if (scanf("%lf", &data[i].value) != 1) {
            printf("❌ Ошибка ввода!\n");
            clear_input_buffer();
            destroy_matrix(m);
            return NULL;
        }
    }

    return m;
}
Matrix* input_double_vector(int size) {
    Matrix* v = create_double_matrix(size, NULL);  
    if (!v) return NULL;

    Double* data = (Double*)v->data;
    printf("Введите %d элементов вектора (вещественные числа):\n", size);

    for (int i = 0; i < size; i++) {
        printf("  b[%d] = ", i);
        if (scanf("%lf", &data[i].value) != 1) {
            printf("❌ Ошибка ввода!\n");
            clear_input_buffer();
            destroy_matrix(v);
            return NULL;
        }
    }
    return v;
}

Integer input_integer_scalar(void) {
    Integer scalar;
    printf("Введите скаляр (целое число): ");
    if (scanf("%d", &scalar.value) != 1) {
        scalar.value = 1;
        clear_input_buffer();
    }
    return scalar;
}

Complex input_complex_scalar(void) {
    Complex scalar;
    printf("Введите скаляр (комплексное число, re im): ");
    if (scanf("%d %d", &scalar.re, &scalar.im) != 2) {
        scalar.re = 1;
        scalar.im = 0;
        clear_input_buffer();
    }
    return scalar;
}

// menu
void show_main_menu(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║     POLYMORPHIC MATRIX SYSTEM v2.0    ║\n");
    printf("║     Full Ring + LU + QR + SLAU        ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  1. Операции с Integer матрицами      ║\n");
    printf("║  2. Операции с Complex матрицами      ║\n");
    printf("║  3. LU-разложение (Double)            ║\n");
    printf("║  4. QR-разложение (Double)            ║\n");
    printf("║  5. Решение СЛАУ (A×x=b)              ║\n");
    printf("║  6. Бенчмарк: LU vs QR                ║\n");
    printf("║  7. Проверка аксиом кольца            ║\n");
    printf("║  8. Запустить все тесты               ║\n");
    printf("║  9. Демо типобезопасности             ║\n");
    printf("║  0. Выход                             ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Выбор: ");
}

void show_integer_menu(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║        INTEGER MATRIX OPERATIONS      ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  1. Создать матрицу вручную           ║\n");
    printf("║  2. Сложение двух матриц              ║\n");
    printf("║  3. Умножение двух матриц             ║\n");
    printf("║  4. Умножение на скаляр               ║\n");
    printf("║  5. Быстрое демо (готовые матрицы)    ║\n");
    printf("║  0. Назад в главное меню              ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Выбор: ");
}

void show_complex_menu(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║        COMPLEX MATRIX OPERATIONS      ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  1. Создать матрицу вручную           ║\n");
    printf("║  2. Сложение двух матриц              ║\n");
    printf("║  3. Умножение двух матриц             ║\n");
    printf("║  4. Умножение на скаляр               ║\n");
    printf("║  5. Быстрое демо (готовые матрицы)    ║\n");
    printf("║  0. Назад в главное меню              ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Выбор: ");
}

// integer
void integer_manual_create(void) {
    int size;
    printf("Введите размер матрицы (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("❌ Некорректный размер!\n");
        clear_input_buffer();
        return;
    }

    Matrix* m = input_integer_matrix(size);
    if (m) {
        print_integer_matrix(m, "Ваша матрица");
        destroy_matrix(m);
    }
}

void integer_add_manual(void) {
    int size;
    printf("Введите размер матриц (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("❌ Некорректный размер!\n");
        clear_input_buffer();
        return;
    }

    printf("\n=== Матрица A ===\n");
    Matrix* A = input_integer_matrix(size);
    if (!A) return;

    printf("\n=== Матрица B ===\n");
    Matrix* B = input_integer_matrix(size);
    if (!B) {
        destroy_matrix(A);
        return;
    }

    Matrix* C = create_integer_matrix(size, NULL);
    if (!C) {
        print_error("Создание матрицы C", ERR_OUT_OF_MEMORY);
        destroy_matrix(A);
        destroy_matrix(B);
        return;
    }

    ErrorCode err = matrix_add(A, B, C);
    if (err != ERR_OK) {
        print_error("Сложение матриц", err);
        destroy_matrix(A);
        destroy_matrix(B);
        destroy_matrix(C);
        return;
    }

    printf("\n=== Результат ===\n");
    print_integer_matrix(A, "A");
    print_integer_matrix(B, "B");
    print_integer_matrix(C, "A + B");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
}

void integer_multiply_manual(void) {
    int size;
    printf("Введите размер матриц (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("❌ Некорректный размер!\n");
        clear_input_buffer();
        return;
    }

    printf("\n=== Матрица A ===\n");
    Matrix* A = input_integer_matrix(size);
    if (!A) return;

    printf("\n=== Матрица B ===\n");
    Matrix* B = input_integer_matrix(size);
    if (!B) {
        destroy_matrix(A);
        return;
    }

    Matrix* C = create_integer_matrix(size, NULL);
    if (!C) {
        print_error("Создание матрицы C", ERR_OUT_OF_MEMORY);
        destroy_matrix(A);
        destroy_matrix(B);
        return;
    }

    ErrorCode err = matrix_multiply(A, B, C);
    if (err != ERR_OK) {
        print_error("Умножение матриц", err);
        destroy_matrix(A);
        destroy_matrix(B);
        destroy_matrix(C);
        return;
    }

    printf("\n=== Результат ===\n");
    print_integer_matrix(A, "A");
    print_integer_matrix(B, "B");
    print_integer_matrix(C, "A × B");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
}

void integer_scalar_manual(void) {
    int size;
    printf("Введите размер матрицы (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("❌ Некорректный размер!\n");
        clear_input_buffer();
        return;
    }

    printf("\n=== Матрица A ===\n");
    Matrix* A = input_integer_matrix(size);
    if (!A) return;

    Integer scalar = input_integer_scalar();

    Matrix* C = create_integer_matrix(size, NULL);
    if (!C) {
        print_error("Создание матрицы C", ERR_OUT_OF_MEMORY);
        destroy_matrix(A);
        return;
    }

    ErrorCode err = matrix_multiply_scalar(A, &scalar, C);
    if (err != ERR_OK) {
        print_error("Умножение на скаляр", err);
        destroy_matrix(A);
        destroy_matrix(C);
        return;
    }

    printf("\n=== Результат ===\n");
    print_integer_matrix(A, "A");
    printf("Скаляр: %d\n", scalar.value);
    print_integer_matrix(C, "A × scalar");

    destroy_matrix(A);
    destroy_matrix(C);
}

void integer_demo(void) {
    printf("\n=== Integer Matrix Quick Demo ===\n");
    int a[] = {1, 2, 3, 4};
    int b[] = {5, 6, 7, 8};

    Matrix* A = create_integer_matrix(2, a);
    Matrix* B = create_integer_matrix(2, b);
    Matrix* C = create_integer_matrix(2, NULL);

    print_integer_matrix(A, "A");
    print_integer_matrix(B, "B");

    matrix_add(A, B, C);
    print_integer_matrix(C, "A + B");

    matrix_multiply(A, B, C);
    print_integer_matrix(C, "A × B");

    Integer scalar = {.value = 2};
    matrix_multiply_scalar(A, &scalar, C);
    print_integer_matrix(C, "A × 2");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
}

void integer_menu(void) {
    int choice;
    do {
        show_integer_menu();
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 1:
                integer_manual_create();
                break;
            case 2:
                integer_add_manual();
                break;
            case 3:
                integer_multiply_manual();
                break;
            case 4:
                integer_scalar_manual();
                break;
            case 5:
                integer_demo();
                break;
            case 0:
                printf("Возврат в главное меню...\n");
                break;
            default:
                printf("❌ Неверный выбор!\n");
        }
    } while (choice != 0);
}

// complex
void complex_manual_create(void) {
    int size;
    printf("Введите размер матрицы (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("❌ Некорректный размер!\n");
        clear_input_buffer();
        return;
    }

    Matrix* m = input_complex_matrix(size);
    if (m) {
        print_complex_matrix(m, "Ваша матрица");
        destroy_matrix(m);
    }
}

void complex_add_manual(void) {
    int size;
    printf("Введите размер матриц (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("❌ Некорректный размер!\n");
        clear_input_buffer();
        return;
    }

    printf("\n=== Матрица A ===\n");
    Matrix* A = input_complex_matrix(size);
    if (!A) return;

    printf("\n=== Матрица B ===\n");
    Matrix* B = input_complex_matrix(size);
    if (!B) {
        destroy_matrix(A);
        return;
    }

    Matrix* C = create_complex_matrix(size, NULL, NULL);
    if (!C) {
        print_error("Создание матрицы C", ERR_OUT_OF_MEMORY);
        destroy_matrix(A);
        destroy_matrix(B);
        return;
    }

    ErrorCode err = matrix_add(A, B, C);
    if (err != ERR_OK) {
        print_error("Сложение матриц", err);
        destroy_matrix(A);
        destroy_matrix(B);
        destroy_matrix(C);
        return;
    }

    printf("\n=== Результат ===\n");
    print_complex_matrix(A, "A");
    print_complex_matrix(B, "B");
    print_complex_matrix(C, "A + B");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
}

void complex_multiply_manual(void) {
    int size;
    printf("Введите размер матриц (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("❌ Некорректный размер!\n");
        clear_input_buffer();
        return;
    }

    printf("\n=== Матрица A ===\n");
    Matrix* A = input_complex_matrix(size);
    if (!A) return;

    printf("\n=== Матрица B ===\n");
    Matrix* B = input_complex_matrix(size);
    if (!B) {
        destroy_matrix(A);
        return;
    }

    Matrix* C = create_complex_matrix(size, NULL, NULL);
    if (!C) {
        print_error("Создание матрицы C", ERR_OUT_OF_MEMORY);
        destroy_matrix(A);
        destroy_matrix(B);
        return;
    }

    ErrorCode err = matrix_multiply(A, B, C);
    if (err != ERR_OK) {
        print_error("Умножение матриц", err);
        destroy_matrix(A);
        destroy_matrix(B);
        destroy_matrix(C);
        return;
    }

    printf("\n=== Результат ===\n");
    print_complex_matrix(A, "A");
    print_complex_matrix(B, "B");
    print_complex_matrix(C, "A × B");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);
}

void complex_scalar_manual(void) {
    int size;
    printf("Введите размер матрицы (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("❌ Некорректный размер!\n");
        clear_input_buffer();
        return;
    }

    printf("\n=== Матрица A ===\n");
    Matrix* A = input_complex_matrix(size);
    if (!A) return;

    Complex scalar = input_complex_scalar();

    Matrix* C = create_complex_matrix(size, NULL, NULL);
    if (!C) {
        print_error("Создание матрицы C", ERR_OUT_OF_MEMORY);
        destroy_matrix(A);
        return;
    }

    ErrorCode err = matrix_multiply_scalar(A, &scalar, C);
    if (err != ERR_OK) {
        print_error("Умножение на скаляр", err);
        destroy_matrix(A);
        destroy_matrix(C);
        return;
    }

    printf("\n=== Результат ===\n");
    print_complex_matrix(A, "A");
    printf("Скаляр: %d+%di\n", scalar.re, scalar.im);
    print_complex_matrix(C, "A × scalar");

    destroy_matrix(A);
    destroy_matrix(C);
}

void complex_demo(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║     COMPLEX MATRIX QUICK DEMO         ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    Matrix* A =
        create_complex_matrix(2, (int[]){1, 0, 1, 2}, (int[]){0, 1, 1, 0});
    Matrix* B =
        create_complex_matrix(2, (int[]){1, 1, 0, 1}, (int[]){0, 0, 1, 1});
    Matrix* C = create_complex_matrix(2, NULL, NULL);

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  1. MATRIX ADDITION (A + B)           ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    print_complex_matrix(A, "A");
    print_complex_matrix(B, "B");

    matrix_add(A, B, C);
    print_complex_matrix(C, "A + B");

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  2. MATRIX MULTIPLICATION (A × B)     ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    print_complex_matrix(A, "A");
    print_complex_matrix(B, "B");

    matrix_multiply(A, B, C);
    print_complex_matrix(C, "A × B");

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  3. SCALAR MULTIPLICATION (A × s)     ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    print_complex_matrix(A, "A");
    Complex scalar = {.re = 2, .im = 1};
    printf("Scalar: %d+%di\n", scalar.re, scalar.im);

    matrix_multiply_scalar(A, &scalar, C);
    print_complex_matrix(C, "A × scalar");

    destroy_matrix(A);
    destroy_matrix(B);
    destroy_matrix(C);

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║         DEMO COMPLETED                ║\n");
    printf("╚═══════════════════════════════════════╝\n");
}

void complex_menu(void) {
    int choice;
    do {
        show_complex_menu();
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 1:
                complex_manual_create();
                break;
            case 2:
                complex_add_manual();
                break;
            case 3:
                complex_multiply_manual();
                break;
            case 4:
                complex_scalar_manual();
                break;
            case 5:
                complex_demo();
                break;
            case 0:
                printf("Возврат в главное меню...\n");
                break;
            default:
                printf("❌ Неверный выбор!\n");
        }
    } while (choice != 0);
}

// type
void demo_type_safety(void) {
    printf("\n=== Type Safety Demo ===\n");
    Matrix* IntM = create_integer_matrix(2, (int[]){1, 2, 3, 4});
    Matrix* CompM =
        create_complex_matrix(2, (int[]){1, 2, 3, 4}, (int[]){0, 0, 0, 0});

    printf("Integer ops pointer: %p\n", (void*)IntM->operations);
    printf("Complex ops pointer: %p\n", (void*)CompM->operations);
    printf("Pointers equal: %s\n",
           (IntM->operations == CompM->operations) ? "YES" : "NO");

    printf("\nAttempting to add Integer + Complex (should fail)...\n");
    Matrix* Result = create_integer_matrix(2, NULL);

    ErrorCode err = matrix_add(IntM, CompM, Result);
    if (err != ERR_OK) {
        printf("✅ Ожидаемая ошибка: %s\n", error_message(err));
    }

    destroy_matrix(IntM);
    destroy_matrix(CompM);
    destroy_matrix(Result);
}

void ring_axioms_demo(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  RING AXIOMS DEMO                     ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    printf("\n[1] Integer Ring\n");
    printf("[2] Double Field\n");
    printf("[3] Complex Ring\n");
    printf("Выбор: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return;
    }

    const AlgebraOperations* ops = NULL;
    const char* name = "";

    switch (choice) {
        case 1:
            ops = GetIntegerOps();
            name = "Integer";
            break;
        case 2:
            ops = GetDoubleOps();
            name = "Double";
            break;
        case 3:
            ops = GetComplexOps();
            name = "Complex";
            break;
        default:
            printf("❌ Неверный выбор!\n");
            return;
    }

    printf("\nПроверка аксиом кольца для типа: %s\n", name);
    test_ring_axioms(ops);
}

void lu_decomposition_demo(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║     LU DECOMPOSITION DEMO             ║\n");
    printf("║     A = L × U                         ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    printf("\n[1] Использовать готовую матрицу для демо\n");
    printf("[2] Ввести матрицу вручную\n");
    printf("Выбор: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return;
    }

    Matrix* A = NULL;

    if (choice == 1) {
        double a_vals[] = {4.0, 3.0, 2.0, 6.0, 5.0, 4.0, 2.0, 1.0, 3.0};
        A = create_double_matrix(3, a_vals);
        printf("Использована готовая матрица 3×3\n");
    } else {
        int size;
        printf("Введите размер матрицы (n для n×n, макс. 5): ");
        if (scanf("%d", &size) != 1 || size < 2 || size > 5) {
            printf("❌ Некорректный размер!\n");
            clear_input_buffer();
            return;
        }
        A = input_double_matrix(size);
        if (!A) return;
    }

    if (!A) {
        print_error("Создание матрицы A", ERR_OUT_OF_MEMORY);
        return;
    }

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  Исходная матрица A:                  ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    print_double_matrix(A, "A");

    Matrix* L = create_double_matrix(A->size, NULL);
    Matrix* U = create_double_matrix(A->size, NULL);

    if (!L || !U) {
        print_error("Создание матриц L/U", ERR_OUT_OF_MEMORY);
        destroy_matrix(A);
        destroy_matrix(L);
        destroy_matrix(U);
        return;
    }

    printf("\nВыполнение LU-разложения...\n");

    clock_t start = clock();
    ErrorCode result = matrix_lu_decompose(A, L, U);
    clock_t end = clock();
    double time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    if (result == ERR_OK) {
        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  Нижняя треугольная матрица L:        ║\n");
        printf("║  (единицы на диагонали)               ║\n");
        printf("╚═══════════════════════════════════════╝\n");
        print_double_matrix(L, "L");

        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  Верхняя треугольная матрица U:       ║\n");
        printf("╚═══════════════════════════════════════╝\n");
        print_double_matrix(U, "U");

        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  Проверка: L × U == A ?               ║\n");
        printf("╚═══════════════════════════════════════╝\n");

        Matrix* LU = create_double_matrix(A->size, NULL);
        if (LU) {
            matrix_multiply(L, U, LU);
            print_double_matrix(LU, "L × U");

            Double* a_data = (Double*)A->data;
            Double* lu_data = (Double*)LU->data;
            int n = A->size;
            int match = 1;
            for (int i = 0; i < n * n; i++) {
                if (fabs(a_data[i].value - lu_data[i].value) > 1e-9) {
                    match = 0;
                    break;
                }
            }

            printf("\nРезультат проверки: %s\n",
                   match ? "✅ PASS (L×U совпадает с A)"
                         : "⚠️  Небольшое расхождение (погрешность float)");
            printf("\n⏱  Время выполнения LU: %.3f мс\n", time_ms);

            destroy_matrix(LU);
        }

    } else if (result == ERR_SINGULAR_MATRIX) {
        printf("\n❌ Ошибка: %s\n", error_message(result));
        printf("   LU-разложение без выбора ведущего элемента невозможно.\n");
        printf("⏱  Время до обнаружения ошибки: %.3f мс\n", time_ms);
    } else {
        printf("\n❌ Ошибка LU-разложения: %s (код: %d)\n",
               error_message(result), result);
        printf("⏱  Время выполнения: %.3f мс\n", time_ms);
    }

    destroy_matrix(A);
    destroy_matrix(L);
    destroy_matrix(U);

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║         DEMO COMPLETED                ║\n");
    printf("╚═══════════════════════════════════════╝\n");
}

void qr_decomposition_demo(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║     QR DECOMPOSITION DEMO             ║\n");
    printf("║     A = Q × R                         ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    printf("\n[1] Использовать готовую матрицу для демо\n");
    printf("[2] Ввести матрицу вручную (только Double)\n");
    printf("Выбор: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return;
    }

    Matrix* A = NULL;

    if (choice == 1) {
        double a_vals[] = {4.0, 3.0, 2.0, 6.0, 5.0, 4.0, 2.0, 1.0, 3.0};
        A = create_double_matrix(3, a_vals);
        printf("✅ Использована готовая матрица 3×3 (Double)\n");
    } else {
        int size;
        printf("Введите размер матрицы (n для n×n, макс. 5): ");
        if (scanf("%d", &size) != 1 || size < 2 || size > 5) {
            printf("❌ Некорректный размер!\n");
            clear_input_buffer();
            return;
        }
        A = input_double_matrix(size);
        if (!A) return;
    }

    if (!A) {
        printf("❌ Ошибка создания матрицы!\n");
        return;
    }

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  Исходная матрица A:                  ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    print_double_matrix(A, "A");

    Matrix* Q = create_double_matrix(A->size, NULL);
    Matrix* R = create_double_matrix(A->size, NULL);

    if (!Q || !R) {
        printf("❌ Ошибка выделения памяти для Q/R!\n");
        destroy_matrix(A);
        destroy_matrix(Q);
        destroy_matrix(R);
        return;
    }

    printf("\nВыполнение QR-разложения...\n");
    clock_t start = clock();
    ErrorCode result = matrix_qr_decompose(A, Q, R);
    clock_t end = clock();
    double time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    if (result == ERR_OK) {
        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  Ортогональная матрица Q:             ║\n");
        printf("║  (Q^T × Q = I)                        ║\n");
        printf("╚═══════════════════════════════════════╝\n");
        print_double_matrix(Q, "Q");

        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  Верхняя треугольная матрица R:       ║\n");
        printf("╚═══════════════════════════════════════╝\n");
        print_double_matrix(R, "R");

        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  Проверка: Q × R == A ?               ║\n");
        printf("╚═══════════════════════════════════════╝\n");

        Matrix* QR = create_double_matrix(A->size, NULL);
        if (QR) {
            matrix_multiply(Q, R, QR);
            print_double_matrix(QR, "Q × R");

            Double* a_data = (Double*)A->data;
            Double* qr_data = (Double*)QR->data;
            int n = A->size;
            int match = 1;
            for (int i = 0; i < n * n; i++) {
                if (fabs(a_data[i].value - qr_data[i].value) > 1e-9) {
                    match = 0;
                    break;
                }
            }

            printf("\nРезультат проверки: %s\n",
                   match ? "✅ PASS (Q×R совпадает с A)"
                         : "⚠️  Небольшое расхождение (погрешность float)");
            printf("⏱  Время выполнения QR: %.3f мс\n", time_ms);

            destroy_matrix(QR);
        }
    } else if (result == ERR_SINGULAR_MATRIX) {
        printf("\n❌ Ошибка: %s\n", error_message(result));
        printf("   QR-разложение невозможно (линейно зависимые столбцы)\n");
        printf("⏱  Время до обнаружения ошибки: %.3f мс\n", time_ms);
    } else {
        printf("\n❌ Ошибка QR-разложения: %s (код: %d)\n", 
               error_message(result), result);
        printf("⏱  Время выполнения: %.3f мс\n", time_ms);
    }

    destroy_matrix(A);
    destroy_matrix(Q);
    destroy_matrix(R);

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║         DEMO COMPLETED                ║\n");
    printf("╚═══════════════════════════════════════╝\n");
}


void solve_slau_demo(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║     SOLVE SLAU: A × x = b             ║\n");
    printf("║     Методы: LU и QR                   ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    printf("\n[1] Готовая система 2×2 (Double)\n");
    printf("[2] Готовая система 3×3 (Double)\n");
    printf("[3] Ввести свою систему (только Double)\n");
    printf("Выбор: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return;
    }

    Matrix *A = NULL, *b = NULL;

    if (choice == 1) {
        // 2x + y = 5; x + 3y = 6 → x=1.8, y=1.4
        printf("Система: 2x + y = 5,  x + 3y = 6\n");
        A = create_double_matrix(2, (double[]){2, 1, 1, 3});
        b = create_double_matrix(2, (double[]){5, 6});
    } else if (choice == 2) {
        // 4x+3y+2z=9, 6x+5y+4z=15, 2x+y+3z=6
        printf("Система: 4x+3y+2z=9,  6x+5y+4z=15,  2x+y+3z=6\n");
        A = create_double_matrix(3, (double[]){4,3,2, 6,5,4, 2,1,3});
        b = create_double_matrix(3, (double[]){9, 15, 6});
    } else {
        int size;
        printf("Размер системы (n): ");
        if (scanf("%d", &size) != 1 || size < 2 || size > 5) {
            printf("❌ Некорректный размер!\n");
            clear_input_buffer();
            return;
        }
        printf("\n=== Ввод матрицы A ===\n");
        A = input_double_matrix(size);
        printf("\n=== Ввод вектора b ===\n");
        b = input_double_vector(size);  
        if (!A || !b) {
            destroy_matrix(A); destroy_matrix(b);
            return;
        }
    }

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  Матрица A:                           ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    print_double_matrix(A, "A");
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  Вектор b:                            ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    print_double_vector(b, "b");

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  РЕШЕНИЕ ЧЕРЕЗ LU                     ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    Matrix* x_lu = create_double_matrix(A->size, NULL);
    
    clock_t lu_start = clock();
    ErrorCode lu_err = solve_lu(A, b, x_lu);
    clock_t lu_end = clock();
    double lu_time = (double)(lu_end - lu_start) * 1000.0 / CLOCKS_PER_SEC;

    if (lu_err == ERR_OK) {
        print_double_vector(x_lu, "x (LU)");
        printf("⏱  Время: %.3f мс\n", lu_time);
    } else {
        printf("❌ Ошибка LU: %s\n", error_message(lu_err));
    }

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  РЕШЕНИЕ ЧЕРЕЗ QR                     ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    Matrix* x_qr = create_double_matrix(A->size, NULL);
    
    clock_t qr_start = clock();
    ErrorCode qr_err = solve_qr(A, b, x_qr);
    clock_t qr_end = clock();
    double qr_time = (double)(qr_end - qr_start) * 1000.0 / CLOCKS_PER_SEC;

    if (qr_err == ERR_OK) {
        print_double_vector(x_qr, "x (QR)");
        printf("⏱  Время: %.3f мс\n", qr_time);
    } else {
        printf("❌ Ошибка QR: %s\n", error_message(qr_err));
    }

    if (lu_err == ERR_OK && qr_err == ERR_OK) {
        Double* xlu = (Double*)x_lu->data;
        Double* xqr = (Double*)x_qr->data;
        double max_diff = 0.0;
        for (int i = 0; i < A->size; i++) {
            double diff = fabs(xlu[i].value - xqr[i].value);
            if (diff > max_diff) max_diff = diff;
        }
        printf("\n╔═══════════════════════════════════════╗\n");
        printf("║  СРАВНЕНИЕ РЕШЕНИЙ                    ║\n");
        printf("╚═══════════════════════════════════════╝\n");
        printf("Максимальное расхождение: %.2e\n", max_diff);
        printf("Статус: %s\n",
               (max_diff < 1e-8) ? "✅ Решения совпадают" : "⚠️  Небольшое расхождение");
    }

    destroy_matrix(A);
    destroy_matrix(b);
    destroy_matrix(x_lu);
    destroy_matrix(x_qr);

    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║         DEMO COMPLETED                ║\n");
    printf("╚═══════════════════════════════════════╝\n");
}


void benchmark_demo(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  BENCHMARK: LU vs QR                  ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    printf("\nВыберите размер матрицы:\n");
    printf("[1] 3×3\n");
    printf("[2] 5×5\n");
    printf("[3] 10×10\n");
    printf("Выбор: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return;
    }

    int size = (choice == 1) ? 3 : (choice == 2) ? 5 : 10;
    benchmark_lu_vs_qr(size);
}
// run main
void run_interactive_mode(void) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║  POLYMORPHIC MATRIX SYSTEM             ║\n");
    printf("║  Variant 21: Integer & Complex         ║\n");
    printf("╚════════════════════════════════════════╝\n");

    int choice;
    do {
        show_main_menu();
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 1:
                integer_menu();
                break;
            case 2:
                complex_menu();
                break;
            case 3:
                lu_decomposition_demo();
                break;
            case 4:
                qr_decomposition_demo();  
                break;
            case 5:
                solve_slau_demo();  
                break;
            case 6:
                benchmark_demo();  
                break;
            case 7:
                ring_axioms_demo();
                break;
            case 8:
                run_all_tests();
                break;
            case 9:
                demo_type_safety();
                break;
            case 0:
                printf("exit!\n");
                break;
            default:
                printf("❌ Invalid choice!\n");
}
    } while (choice != 0);
}