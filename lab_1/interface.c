#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tests.h"


void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}



Matrix* input_integer_matrix(int size) {
    Matrix* m = create_integer_matrix(size, NULL);
    if (!m) return NULL;
    
    Integer* data = (Integer*)m->data;
    printf("Введите %d элементов матрицы %dx%d (целые числа):\n", size * size, size, size);
    
    for (int i = 0; i < size * size; i++) {
        int row = i / size;
        int col = i % size;
        printf("  [%d][%d] = ", row, col);
        if (scanf("%d", &data[i].value) != 1) {
            fprintf(stderr, "Ошибка ввода!\n");
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
    printf("Введите %d элементов матрицы %dx%d (комплексные числа):\n", size * size, size, size);
    
    for (int i = 0; i < size * size; i++) {
        int row = i / size;
        int col = i % size;
        printf("  [%d][%d] (re im) = ", row, col);
        if (scanf("%d %d", &data[i].re, &data[i].im) != 2) {
            fprintf(stderr, "Ошибка ввода!\n");
            clear_input_buffer();
            destroy_matrix(m);
            return NULL;
        }
    }
    
    return m;
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
    printf("║     POLYMORPHIC MATRIX SYSTEM v1.0    ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  1. Операции с Integer матрицами      ║\n");
    printf("║  2. Операции с Complex матрицами      ║\n");
    printf("║  3. Запустить все тесты               ║\n");
    printf("║  4. Демо типобезопасности             ║\n");
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
        printf("Некорректный размер!\n");
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
        printf("Некорректный размер!\n");
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
        destroy_matrix(A);
        destroy_matrix(B);
        return;
    }
    
    matrix_add(A, B, C);
    
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
        printf("Некорректный размер!\n");
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
        destroy_matrix(A);
        destroy_matrix(B);
        return;
    }
    
    matrix_multiply(A, B, C);
    
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
        printf("Некорректный размер!\n");
        clear_input_buffer();
        return;
    }
    
    printf("\n=== Матрица A ===\n");
    Matrix* A = input_integer_matrix(size);
    if (!A) return;
    
    Integer scalar = input_integer_scalar();
    
    Matrix* C = create_integer_matrix(size, NULL);
    if (!C) {
        destroy_matrix(A);
        return;
    }
    
    matrix_multiply_scalar(A, &scalar, C);
    
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
            case 1: integer_manual_create(); break;
            case 2: integer_add_manual(); break;
            case 3: integer_multiply_manual(); break;
            case 4: integer_scalar_manual(); break;
            case 5: integer_demo(); break;
            case 0: printf("Возврат в главное меню...\n"); break;
            default: printf("Неверный выбор!\n");
        }
    } while (choice != 0);
}


// complex
void complex_manual_create(void) {
    int size;
    printf("Введите размер матрицы (n для n×n): ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("Некорректный размер!\n");
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
        printf("Некорректный размер!\n");
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
        destroy_matrix(A);
        destroy_matrix(B);
        return;
    }
    
    matrix_add(A, B, C);
    
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
        printf("Некорректный размер!\n");
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
        destroy_matrix(A);
        destroy_matrix(B);
        return;
    }
    
    matrix_multiply(A, B, C);
    
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
        printf("Некорректный размер!\n");
        clear_input_buffer();
        return;
    }
    
    printf("\n=== Матрица A ===\n");
    Matrix* A = input_complex_matrix(size);
    if (!A) return;
    
    Complex scalar = input_complex_scalar();
    
    Matrix* C = create_complex_matrix(size, NULL, NULL);
    if (!C) {
        destroy_matrix(A);
        return;
    }
    
    matrix_multiply_scalar(A, &scalar, C);
    
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
    
    // Матрица A = [[1+0i, 0+1i], [1+1i, 2+0i]]
    Matrix* A = create_complex_matrix(2, 
        (int[]){1, 0, 1, 2},  // re
        (int[]){0, 1, 1, 0}   // im
    );
    
    // Матрица B = [[1+0i, 1+0i], [0+1i, 1+1i]]
    Matrix* B = create_complex_matrix(2, 
        (int[]){1, 1, 0, 1},  // re
        (int[]){0, 0, 1, 1}   // im
    );
    
    // Матрица C для результатов
    Matrix* C = create_complex_matrix(2, NULL, NULL);
    
    // ========================================================================
    // 1. Сложение матриц
    // ========================================================================
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  1. MATRIX ADDITION (A + B)           ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    
    print_complex_matrix(A, "A");
    print_complex_matrix(B, "B");
    
    matrix_add(A, B, C);
    print_complex_matrix(C, "A + B");
    
    // ========================================================================
    // 2. Умножение матриц
    // ========================================================================
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  2. MATRIX MULTIPLICATION (A × B)     ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    
    print_complex_matrix(A, "A");
    print_complex_matrix(B, "B");
    
    matrix_multiply(A, B, C);
    print_complex_matrix(C, "A × B");
    
    // ========================================================================
    // 3. Умножение на скаляр
    // ========================================================================
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  3. SCALAR MULTIPLICATION (A × s)     ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    
    print_complex_matrix(A, "A");
    Complex scalar = {.re = 2, .im = 1};  // Скаляр: 2+1i
    printf("Scalar: %d+%di\n", scalar.re, scalar.im);
    
    matrix_multiply_scalar(A, &scalar, C);
    print_complex_matrix(C, "A × scalar");
    
    // ========================================================================
    // Очистка памяти
    // ========================================================================
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
            case 1: complex_manual_create(); break;
            case 2: complex_add_manual(); break;
            case 3: complex_multiply_manual(); break;
            case 4: complex_scalar_manual(); break;
            case 5: complex_demo(); break;
            case 0: printf("Возврат в главное меню...\n"); break;
            default: printf("Неверный выбор!\n");
        }
    } while (choice != 0);
}


// type
void demo_type_safety(void) {
    printf("\n=== Type Safety Demo ===\n");
    Matrix* IntM = create_integer_matrix(2, (int[]){1, 2, 3, 4});
    Matrix* CompM = create_complex_matrix(2, (int[]){1, 2, 3, 4}, (int[]){0, 0, 0, 0});
    
    printf("Integer ops pointer: %p\n", (void*)IntM->operations);
    printf("Complex ops pointer: %p\n", (void*)CompM->operations);
    printf("Pointers equal: %s\n", (IntM->operations == CompM->operations) ? "YES" : "NO");
    
    printf("\nAttempting to add Integer + Complex (should fail)...\n");
    Matrix* Result = create_integer_matrix(2, NULL);
    matrix_add(IntM, CompM, Result);
    
    destroy_matrix(IntM);
    destroy_matrix(CompM);
    destroy_matrix(Result);
}


// run main
void run_interactive_mode(void) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║  POLYMORPHIC MATRIX SYSTEM v1.0        ║\n");
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
            case 1: integer_menu(); break;
            case 2: complex_menu(); break;
            case 3: run_all_tests(); break;
            case 4: demo_type_safety(); break;
            case 0: printf("exit!\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 0);
}