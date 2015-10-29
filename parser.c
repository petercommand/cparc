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
  list* static_context = list_new();
  char* elem;
  while(*list) {
    elem = malloc(sizeof(char));
    *elem = *list;
    list_push_back(static_context, elem);
    list++;
  }
  
}

parser* parser_new(static_context* sc, dynamic_parser* dp, bool allow_empty) {
  parser* p = (parser *)calloc(sizeof(parser));
  p.status = PARSER_NORMAL;
  p.sc = &sc;
  p.dp = dp;
  p.allow_empty = allow_empty;
  return p;
}

void parser_delete(parser* p) {
  free(p);
}
