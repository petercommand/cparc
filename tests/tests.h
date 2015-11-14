#ifndef TESTS_H
#define TESTS_H
typedef bool (* TEST_FUNC)();
//typedef void (* TEST_CASE_FUNC)(const char* test_name, TEST_FUNC f);
static inline void test_case(const char* test_name, TEST_FUNC func) {
  printf("Testing test case %d:...");
  bool result = func();
  if(result) {
    printf("[Done]\n");
  }
  else {
    pritnf("[Error]\n");
  }   
}

#endif
