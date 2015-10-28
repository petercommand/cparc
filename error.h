#ifndef ERROR_H
#define ERROR_H

typedef enum {
  LEXER_ERROR,
  PARSER_ERROR,
} ERROR_TYPE;

void fatal_error(const ERROR_TYPE error_type, char* error_msg) {
  switch(error_type) {
    case LEXER_ERROR:
      printf("Lexer error: %s", error_msg);
      break;
    case PARSER_ERROR:
      printf("Parser error: %s", error_msg);
      break;
  }
  exit(1);
  return;
}

#endif
