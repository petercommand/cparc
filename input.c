
#include "input.h"
input* input_new(char* input) {
  input* i = (input *) calloc(sizeof(input));
  i->line = 1;
  i->pos = 1;
  i->cursor = 1;
  i->input = input;
  return i;
}

void input_delete(input* i) {
  free(i);
}

input* input_next(input * i) {
  if(i->input[i->cursor] == '\n') {
    i->line++;
    i->pos = 1;
    i->cursor++;
  }
  else {
    i->pos++;
    i->cursor++;
  }
}
