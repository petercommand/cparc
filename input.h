#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
typedef struct {
  int line;
  int pos;
  int cursor;
  const char* input;
} input_t;
bool input_cmp(input_t* i1, input_t* i2);
void input_init(input_t* i, const char* input);
char input_peek(const input_t* i);
input_t input_next(input_t i);
input_t input_forward(input_t i, int num);

#endif //INPUT_H
