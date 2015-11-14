#ifndef TESTS_H
#define TESTS_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

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

static inline void test_true(bool* result, bool in) {
  if(!in) {
    *result = false;
  }
}

static void test_report() {
  printf("Total: success: %d error: %d\n", total_success, total_error);
}

#endif
