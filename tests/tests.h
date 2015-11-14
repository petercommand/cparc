#ifndef TESTS_H
#define TESTS_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#define test_true(x, y) test_true_imp(x, y, __FILE__, __LINE__)
static int total_success = 0;
static int total_error = 0;
typedef bool (* TEST_FUNC)();
//typedef void (* TEST_CASE_FUNC)(const char* test_name, TEST_FUNC f);

static inline void test_case(const char* test_name, TEST_FUNC func) {
  printf("Testing test case %s:...\n", test_name);
  bool result = func();
  if(result) {
    printf("[Test case %s Success]\n", test_name);
    total_success++;
  }
  else {
    printf("[Test case %s Error]\n", test_name);
    total_error++;
  }   
}

static inline void test_true_imp(bool* result, bool in, const char* file, int line) {
  if(!in) {
    printf("test_true failed at file: %s line: %d\n", file, line);
    *result = false;
  }
}

static void test_report() {
  printf("Total: success: %d error: %d\n", total_success, total_error);
}

#endif
