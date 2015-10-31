
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

char input_peek(input* i) {
  return i->input[i->cursor];
}
input* input_copy(input* i) {
  //only copy positional data, the input stream is not copied
  input* new_input = (input *) calloc(sizeof(input));
  *new_input = *input;
  return new_input;
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
