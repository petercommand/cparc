#ifndef INPUT_H
#define INPUT_H

typedef struct {
  int line;
  int pos;
  int cursor;
  char* input;
} input_t;

input_t* input_new(char* input);
void input_delete(input_t* i);
char input_peek(input_t* i);
input_t* input_copy(input_t* i);
input_t* input_next(input_t* i);
  

#endif //INPUT_H
