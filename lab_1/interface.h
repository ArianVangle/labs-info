#ifndef INTERFACE_H
#define INTERFACE_H

#include "algebra.h"
#include "matrix.h"

void run_interactive_mode(void);
void show_main_menu(void);

void show_integer_menu(void);
void integer_menu(void);

void show_complex_menu(void);
void complex_menu(void);

Matrix* input_integer_matrix(int size);
Matrix* input_complex_matrix(int size);
Matrix* input_double_matrix(int size);
Integer input_integer_scalar(void);
Complex input_complex_scalar(void);
Matrix* input_double_vector(int size);

void integer_manual_create(void);
void integer_add_manual(void);
void integer_multiply_manual(void);
void integer_scalar_manual(void);
void integer_demo(void);

void complex_manual_create(void);
void complex_add_manual(void);
void complex_multiply_manual(void);
void complex_scalar_manual(void);
void complex_demo(void);

void lu_decomposition_demo(void);
void demo_type_safety(void);
void clear_input_buffer(void);

void qr_decomposition_demo(void);
void solve_slau_demo(void);
void benchmark_demo(void);

#endif  // INTERFACE_H