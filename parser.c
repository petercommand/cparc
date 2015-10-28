#include <stdbool.h>

void parse(parser p, token* i, bool can_fail) {
  if(p.status == PARSER_FAILED) {
    if(!can_fail) {
      fatal_error(PARSER_ERROR, p.error_msg);
    }
    return;
  }
  
}

parser* oneof(char* list) {
  while(*list) {
    
}

parser* create_parser(static_context sc, dynamic_parser* dp, bool allow_empty) {
  parser p;
  memset(&p, 0, sizeof(p));
  p.status = PARSER_NORMAL;
  p.sc = &sc;
  p.dp = dp;
  p.allow_empty = allow_empty;
  return p;
}

void init_parser_internal() {
  
}

void finalize_parser_internal() {

}
