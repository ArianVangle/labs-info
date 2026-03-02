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

#endif // TESTS_H