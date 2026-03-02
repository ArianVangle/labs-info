#ifndef INTERFACE_H
#define INTERFACE_H

#include "matrix.h"
#include "algebra.h"


// Запуск интерактивного режима
void run_interactive_mode(void);

// Отображение главного меню
void show_main_menu(void);



// Подменю для Integer матриц
void show_integer_menu(void);
void integer_menu(void);

// Подменю для Complex матриц
void show_complex_menu(void);
void complex_menu(void);


// Ввод матрицы Integer с клавиатуры
Matrix* input_integer_matrix(int size);

// Ввод матрицы Complex с клавиатуры
Matrix* input_complex_matrix(int size);

// Ввод скаляра Integer
Integer input_integer_scalar(void);

// Ввод скаляра Complex
Complex input_complex_scalar(void);

// integer matrix
void integer_manual_create(void);
void integer_add_manual(void);
void integer_multiply_manual(void);
void integer_scalar_manual(void);
void integer_demo(void);


// complex matrix
void complex_manual_create(void);
void complex_add_manual(void);
void complex_multiply_manual(void);
void complex_scalar_manual(void);
void complex_demo(void);



// Демо типобезопасности
void demo_type_safety(void);

// Очистка буфера ввода
void clear_input_buffer(void);

#endif // INTERFACE_H