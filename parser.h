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

enum {
  TYPE_FUNC_PTR,
  TYPE_CLOSURE
} CLOSURE_TYPE;

typedef struct {
  bool allow_empty;
  list* list;
} static_context;

struct dynamic_parser_closure;
typedef struct dynamic_parser_closure dynamic_parser_closure;


typedef struct {
  void* obj;
  input* i; //remaining input
} parser_dp_return;



typedef struct {
  static_context* static1;
  dynamic_parser_closure* dpc1;
  static_context* static2;
  dynamic_parser_closure* dpc2;
} closure_ctx;
typedef parser_dp_return (*dynamic_parser)(dynamic_parser_closure* ctx, parser*, input*);

typedef struct {
  tag_t tag;//closure type
  tag_t ptr1_tag;//first dp type
  tag_t ptr2_tag;//snd dp type
  union {
    closure_ctx* ctx;//a pair of static & dynamic context
  };
  dynamic_parser* dp_ptr;
} dynamic_parser_closure;





typedef struct {
  parser_status status;
  char* error_msg;
  list* static_context;
  dynamic_parser_closure* dpc;
  bool can_fail;
} parser;




enum {
  ELEM_RANGE,
  ELEM_CHAR,
  ELEM_STR,

} STATIC_PARSER_ELEM_TYPE;


#endif
