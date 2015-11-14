#ifndef PARSER_H
#define PARSER_H

#include "input.h"
#include "list.h"
#include <stddef.h>
#include <stdbool.h>
/*
  parser is the main struct of any parser,
  status: this determines whether the parser is in error state
  error_msg: optional field, show the corresponding error msg created by the parser (if can_fail is set, the parser won't report any error, since trying other parser input is possible)
  sc: static_context of the parser, determines which characters are allowed in the first set (*if allow_empty is set, this field will not be used*)
  dp: dynamic_parser:
 */
typedef enum {
  PARSER_NORMAL = 0,
  PARSER_FAILED = 1
} parser_status;

enum {
  TYPE_FUNC_PTR,
  TYPE_CLOSURE
} CLOSURE_TYPE;

typedef struct {
  bool allow_empty;
  bool can_fail;
  list* list;
  size_t ref_count;
} static_context;

struct dynamic_parser_closure;
typedef struct dynamic_parser_closure dynamic_parser_closure;


typedef struct {
  void* obj;
  parser_status status;
  input_t i; //remaining input
} parser_dp_return;

typedef struct {
  static_context* sc;
  dynamic_parser_closure* dpc;
  size_t ref_count;
} closure_ctx;

typedef parser_dp_return (*dynamic_parser)(dynamic_parser_closure* ctx, input_t in);

typedef struct dynamic_parser_closure {
  tag_t tag;//closure type
  union {
    closure_ctx** ctxes;//a pair of static & dynamic context
  };
  dynamic_parser dp_ptr;
  size_t ref_count;
} dynamic_parser_closure;





typedef struct {
  parser_status status;
  char* error_msg;
  static_context* sc;
  dynamic_parser_closure* dpc;
} parser;

typedef struct {
  int lower_bound;
  int upper_bound;
} range_item;

typedef struct {
  int num; //how many chars to compare
  list* range;
} range_criteria;


enum {
  ELEM_RANGE,
  ELEM_CHAR,
  ELEM_STR
} STATIC_PARSER_ELEM_TYPE;


static_context* static_context_new();
void static_context_delete(static_context* sc);
void static_context_add(static_context* sc, void* item, tag_t tag);
static_context* static_context_copy(static_context* sc);
static_context* static_context_from_list(list* list, bool allow_empty);
bool static_match(static_context* sc, input_t* i);
parser_dp_return dynamic_parser_closure_eval(dynamic_parser_closure* closure, input_t input);
parser_dp_return parse(parser* p, input_t i);
dynamic_parser_closure* dynamic_parser_closure_new(dynamic_parser dp, size_t size, ...);
void dynamic_parser_closure_delete(dynamic_parser_closure* dpc);
parser* choice(parser* a, parser* b);
parser* symbol(char sym);
parser* oneof(char* list);
parser* parser_new(static_context* sc, dynamic_parser_closure* dpc);
void parser_delete(parser* p);
char* char_to_ptr(char a);
char ptr_to_char(char* a);
int ptr_to_int(int* a);
int* int_to_ptr(int a);
#endif
