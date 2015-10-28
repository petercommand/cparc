#ifndef PARSER_H
#define PARSER_H

/*
  parser is the main struct of any parser,
  status: this determines whether the parser is in error state
  error_msg: optional field, show the corresponding error msg created by the parser (if can_fail is set, the parser won't report any error, since trying other parser input is possible)
  sc: static_context of the parser, determines which characters are allowed in the first set (*if allow_empty is set, this field will not be used*)
  dp: dynamic_parser:
 */
typedef enum {
  PARSER_FAILED = 0,
  PARSER_NORMAL = 1
} parser_status;

typedef char** static_context;


typedef struct {
  void* obj;
  token* i; //remaining input
} parser_dp_return;

typedef parser_dp_return (*dynamic_parser)(token*);

typedef struct {
  parser_status status;
  char* error_msg;
  static_context* sc;
  dynamic_parser* dp;
  bool allow_empty;
} parser;


#endif
