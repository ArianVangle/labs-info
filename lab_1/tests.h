#ifndef TESTS_H
#define TESTS_H

void run_all_tests(void);

void test_integer_operations(void);
void test_complex_operations(void);
void test_edge_cases(void);
void test_type_safety(void);

void test_complex_multiply_scalar(void);
void test_complex_multiply_real_scalar(void);
void test_complex_multiply_imaginary_unit(void);

void test_ring_integer(void);
void test_ring_double(void);
void test_ring_complex(void);
void test_matrix_subtract(void);
void test_ring_operations(void);

void test_lu_double_simple(void);
void test_lu_double_identity(void);
void test_lu_double_singular(void);
void test_lu_integer_to_double(void);
void test_lu_operations(void);

void test_qr_decompose(void);
void test_solve_lu(void);
void test_solve_qr(void);
void test_qr_decompose(void);

#endif  // TESTS_H