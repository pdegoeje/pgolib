#ifndef MINUNIT_H
#define MINUNIT_H

#include <assert.h>
#include <stdbool.h>

#include "params.h"

/*!
 * \file
 * \brief Minimal C unit testing framework.
 * 
 * Tests are simple functions, which are run from test suites:
 * 
 * ~~~
 * void my_test() {
 *   mu_assert(1 > 0);
 * }
 *
 * void my_test_suite() {
 *   mu_run_test(my_test);
 *   mu_run_test(...);
 * }
 *
 * mu_declare_suite(my_test_suite);
 * ~~~
 * 
 */

/*!
 * \brief Run a test. 
 * 
 * This forks the program, so the test is free to call abort() or die due to segfaults.
 * It also ensures that after the test is completed, all state is reset to what is was before running the test.
 * 
 * \param test The function to test. The function must return void and require no parameters.
 */
#define mu_run_test(test)   mu_run_test_impl(#test, test)

/*!
 * \brief Run a test suite.
 */
#define mu_run_suite(suite) mu_run_suite_impl(#suite, suite)

struct mu_suite {
  const char *name;
  void (*func)();
  struct mu_suite *next;
};

#define mu_declare_suite(suite) \
__attribute__((constructor))  \
static void mu_declare_##suite() { \
  static struct mu_suite s; \
  s.name = #suite; \
  s.func = suite; \
  mu_register_suite_impl(&s); \
}

//! Check an expression for truth.
#define mu_assert(a)             mu_assert_impl(__FILE__, __LINE__, __func__, (a), #a)
#define mu_assert_eq_str(a, b)   mu_assert_eq_str_impl(__FILE__, __LINE__, __func__, a, b)
#define mu_assert_eq_float(a, b) mu_assert_eq_float_impl(__FILE__, __LINE__, __func__, a, b)
#define mu_assert_eq_float_epsilon(a, b, e) mu_assert_eq_float_epsilon_impl(__FILE__, __LINE__, __func__, a, b, e)
#define mu_assert_eq_float_array(a, b, len, dev) mu_assert_eq_float_array_impl(__FILE__, __LINE__, __func__, (a), (b), (len), (dev))

/*! 
 * \brief Compare to values, and abort if they aren't equal.
 * 
 * The values must be of the same type (exactly equal, not just convertible).
 * \param a The expected value.
 * \param b The value to be tested.
 */
//#define mu_assert_eq(a, b)  mu_assert_eq_impl(__FILE__, __LINE__, __func__, a, b)
//! Declare a test_suite for inclusion in mu_run_all_suites().

EXTERN_C_BEGIN

//! Run all test suites previously registered or declared.
void mu_run_all_suites(void);

void mu_run_test_impl(const char *name, void (*test)());
void mu_run_suite_impl(const char *name, void (*suite)());
void mu_register_suite_impl(struct mu_suite *s);
void mu_assert_impl(const char *file, int line, const char *func, bool a, const char *a_str);
void mu_assert_eq_float_impl(const char *file, int line, const char *func, float a, float b);
void mu_assert_eq_float_epsilon_impl(const char *file, int line, const char *func, float a, float b, float e);
void mu_assert_eq_float_array_impl(const char *file, int line, const char *func, float a[], float b[], int len, float dev);
void mu_assert_eq_str_impl(const char *file, int line, const char *func, const char *a, const char *b);

EXTERN_C_END

#endif
