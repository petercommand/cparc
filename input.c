#include "input.h"
#include <memory.h>
#include <stdlib.h>



input_t* input_new(char* input) {
  input_t* i = calloc(1, sizeof(input_t));
  i->line = 1;
  i->pos = 1;
  i->cursor = 1;
  i->input = input;
  return i;
}

void input_delete(input_t* i) {
  free(i);
}

char input_peek(input_t* i) {
  return i->input[i->cursor];
}

input_t* input_copy(input_t* i) {
  //only copy positional data, the input stream is not copied
  input_t* new_input = (input_t *) calloc(1, sizeof(input_t));
  *new_input = *i;
  return new_input;
}
  
input_t* input_next(input_t* i) {
  if(i->input[i->cursor] == '\n') {
    i->line++;
    i->pos = 1;
    i->cursor++;
  }
  else {
    i->pos++;
    i->cursor++;
  }
  return i;
}
