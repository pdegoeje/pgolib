#include "minunit.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "minunit.h"

// All compilers that matter implement \e
#define CSI_RESET  "\e[0m"
#define CSI_BOLD   "\e[1m"
#define CSI_BG_RED "\e[41m"
#define CSI_BG_GREEN "\e[42m"

static int tests_run;
static int tests_ok;

void mu_run_test_impl(const char *name, void (*test)()) { 
  printf("%s\n", name);
  
  if(fork() == 0) {
    test(); 
    _exit(0);
  }
  int rval;
  wait(&rval);
  
  if(rval == 0) {
    tests_ok++;
  }
  tests_run++;
}

void mu_run_suite_impl(const char *name, void (*suite)()) {
  printf(CSI_BOLD "%s" CSI_RESET "\n", name); 
  suite();
}

void mu_assert_impl(const char *file, int line, const char *func, bool a, const char *a_str) {
  if(!a) {
    printf("%s:%d in %s: assertion failed: %s\n", file, line, func, a_str);
    abort();  
  }
}

void mu_assert_eq_float_impl(const char *file, int line, const char *func, float a, float b) {
  if(nextafterf(a, b) != b) {
    printf("%s:%d in %s: %.10g != %.10g\n", file, line, func, a, b);
    abort();  
  }
}

void mu_assert_eq_float_epsilon_impl(const char *file, int line, const char *func, float a, float b, float e) {
  if(fabsf(a - b) > e) {
    printf("%s:%d in %s: %.10g != %.10g\n", file, line, func, a, b);
    abort();  
  }
}

void mu_assert_eq_float_array_impl(const char *file, int line, const char *func, float a[], float b[], int len, float dev) {
  for(int i = 0; i < len; i++) {
    if(fabsf(a[i] - b[i]) > dev) {
      printf("%s:%d in %s: [%d] %.10g != %.10g\n", file, line, func, i, a[i], b[i]);
      abort();
    }
  }
}

void mu_assert_eq_str_impl(const char *file, int line, const char *func, const char *a, const char *b) {
  if(strcmp(a, b) != 0) {
    printf("%s:%d in %s: %s != %s\n", file, line, func, a, b);
    abort();  
  }
}

static struct mu_suite *suites;

void mu_register_suite_impl(struct mu_suite *s) {  
  s->next = suites; 
  suites = s;
}

void mu_run_all_suites() {
  for(struct mu_suite *s = suites; s; s = s->next) {
    mu_run_suite_impl(s->name, s->func);
  }

  printf(CSI_BOLD CSI_BG_RED "%s%d/%d Tests succeeded." CSI_RESET "\n", 
              (tests_ok < tests_run) ? CSI_BG_RED : CSI_BG_GREEN,
              tests_ok, tests_run);
}
