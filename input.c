#include "input.h"
#include <memory.h>
#include <stdlib.h>



void input_init(input_t* i, const char* input) {
  i->line = 1;
  i->pos = 1;
  i->cursor = 0;
  i->input = input;
}



char input_peek(const input_t* i) {
  return i->input[i->cursor];
}

bool input_cmp(input_t* i1, input_t* i2) {
  return
    !((i1->input == i2->input) &&
      (i1->line == i2->line) &&
      (i1->pos == i2->pos) &&
      (i1->cursor == i2->cursor));
}
input_t input_next(input_t i) {
  if(i.input[i.cursor] == '\n') {
    i.line++;
    i.pos = 1;
    i.cursor++;
  }
  else {
    i.pos++;
    i.cursor++;
  }
  return i;
}
input_t input_forward(input_t i, int num) {
  while(num > 0) {
    i = input_next(i);
    num--;
  }
  return i;
}
