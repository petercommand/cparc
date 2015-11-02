#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "parser.h"
#include "list.h"
#include "input.h"

static char char_to_ptr_mapping[256];
static int int_to_ptr_mapping[INT_MAX];

bool match_range_criteria(void* elem, input_t* i);
bool match_char_criteria(char* elem, input_t* i);
bool match_str_criteria(char* elem, input_t* i);
parser_dp_return oneof_dp(dynamic_parser_closure* unused, input_t* in);


parser_dp_return choice_dp(dynamic_parser_closure* dpc, input_t* input);


parser* try_parser(parser* p) {
  
}

static_context* static_context_new() {
  static_context* sc = (static_context *)sizeof(static_context);
  if(sc) {
    sc->list = list_new();
    sc->allow_empty = false;
  }
  return sc;
}

void static_context_delete(static_context* sc) {
  if(sc) {
    list_delete(sc->list);
    free(sc);
  }
}

void static_context_add(static_context* sc, void* item, tag_t tag) {
  list_push_back_with_tag(sc->list, item, tag);
}


static_context* static_context_copy(static_context* sc) {
  static_context* new_sc = static_context_new();
  if(new_sc) {
    new_sc->list = list_copy(sc->list);
    new_sc->allow_empty = sc->allow_empty;
  }
  return new_sc;
}

static_context* static_context_from_list(list* list, bool allow_empty) {
  static_context* sc = static_context_new();
  if(sc) {
    sc->list = list_copy(list);
    sc->allow_empty = allow_empty;
  }
  return sc;
}

bool match_range_criteria(void* elem, input_t* i) {
  
}

bool match_char_criteria(char* elem, input_t* i) {
  if(input_peek(i) == ptr_to_char(elem)) {
    return true;
  }
}

bool match_str_criteria(char* elem, input_t* i) {
  return !strcmp(elem, &(i->input[i->cursor]));
}

bool match_static_criteria(tag_t tag, void* elem, input_t* i) {
  //this function checks whether the input matches the given static criteria
  switch(tag) {
  case ELEM_RANGE:
    return match_range_criteria(elem, i);
  case ELEM_CHAR:
    return match_char_criteria(elem, i);
  case ELEM_STR:
    return match_str_criteria(elem, i);
  default:
    return false;//unrecognized tag
  } 
}

bool static_match(static_context* sc, input_t* i) {
  if(input_peek(i) == '\0' && sc->allow_empty) {
    return true;
  }
  if(input_peek(i) == '\0' && !sc->allow_empty) {
    return false;
  }
  list_item* head = sc->list->head;
  while(head) {
    if(match_static_criteria(head->tag, head->item, i)) {
      return true;
    }
  }
  return false;
}

  

parser_dp_return dynamic_parser_closure_eval(dynamic_parser_closure* closure, input_t* input) {
  switch(closure->tag) {
  case TYPE_FUNC_PTR:
    return closure->dp_ptr(NULL, input);
    break;
  case TYPE_CLOSURE:
    return closure->dp_ptr(closure, input);
    break;
  }

}
parser_dp_return parse(parser* p, input_t* i) {
  if(static_match(p->static_context, i)) {
    return dynamic_parser_closure_eval(p->dpc, i);
  }
  else {
    parser_dp_return failed;
    failed.status = PARSER_FAILED;
    failed.obj = NULL;
    failed.i = i;
    return failed;
  }
}

dynamic_parser_closure* generic_dpc(static_context* static_a, dynamic_parser_closure* dpc_a, static_context* static_b, dynamic_parser_closure* dpc_b, dynamic_parser dp) {
  dynamic_parser_closure* dpc = (dynamic_parser_closure *) calloc(1, sizeof(dynamic_parser_closure));
  dpc->tag = TYPE_CLOSURE;
  dpc->ctx->static1 = static_context_copy(static_a);
  dpc->ctx->dpc1 = dpc_a;
  dpc->ctx->static2 = static_context_copy(static_b);
  dpc->ctx->dpc2 = dpc_b;
  dpc->dp_ptr = dp;
  return dpc;
}

parser* choice(parser* a, parser* b) {
  //choice operator
  static_context* sc = static_context_new();

  list_append(sc->list, a->static_context->list);
  list_append(sc->list, b->static_context->list);
  sc->allow_empty = a->static_context->allow_empty || b->static_context->allow_empty;
  dynamic_parser_closure* dpc = generic_dpc(a->static_context, a->dpc, b->static_context, b->dpc, choice_dp);
  return parser_new(sc, dpc);
}

parser_dp_return choice_dp(dynamic_parser_closure* dpc, input_t* input) {
  if(input_peek(input) == '\0') {
    if(dpc->ctx->static1->allow_empty) {
      return dynamic_parser_closure_eval(dpc->ctx->dpc1, input);
    }
    else {
      return dynamic_parser_closure_eval(dpc->ctx->dpc2, input);
    }
  }
  else {//nonempty input
    parser_dp_return result1 = dynamic_parser_closure_eval(dpc->ctx->dpc1, input);
    if(result1.status == PARSER_NORMAL) {//successful parse
      return result1;
    }
    else {//failed parse
      if(dpc->ctx->static1->can_fail) {//failed parse, try second parser
	return dynamic_parser_closure_eval(dpc->ctx->dpc2, input);
      }
      else {
	return result1;
      }
    }
  }
}

parser_dp_return many_dp(dynamic_parser_closure* dpc, input_t* in) {
  parse_dp_return ret;
  ret.obj = NULL;
  ret.status = PARSER_NORMAL;
  ret.i = in;
  parser_dp_return last_ret;
  while(ret.status == PARSER_NORMAL) {
    last_ret = ret;
    ret = dynamic_parser_closure_eval(dpc->ctx->dpc1, ret.input);
  }
  return last_ret;
}

parser* many(parser* p) {
  static_context* sc = static_cotnext_copy(p->static_cotnext);
  dynamic_parser_closure* dpc = (dynamic_parser_closure *) calloc(1, sizeof(dunamic_parser_closure));
  dpc->tag = TYPE_CLOSURE;
  dpc->dp_ptr = many_dp;
  dpc->static1 = p->static_context;
  dpc->dpc1 = p->dpc;
  return parser_new(sc, dpc);
}

parser_dp_return many1_dp(dynamic_parser_closure* dpc, input_t* in) {
  parser__dp_return ret = many_dp(dpc, in);
  if(ret.i == in) {
    ret.status = PARSER_FAILED;
  }
  return ret;
}

parser* many1(parser* p) {
  static_context* sc = static_context_copy(p->static_context);
  dpc->tag = TYPE_CLOSURE;
  dpc->dp_ptr = many1_dp;
  dpc->static1 = p->static_context;
  dpc->dpc1 = p->dpc;
  return parser_new(sc, dpc);
}

parser* symbol(char sym) {
  static_context* sc = static_context_new();
  char* elem = char_to_ptr(sym);
  //NOT FINISHED
}

parser* oneof(char* list) {
  //create a new parser that consume exactly one char that is in the list
  static_context* sc = static_context_new();
  char* elem;
  while(*list) {
    elem = char_to_ptr(*list);
    static_context_add(sc, elem, ELEM_CHAR);
    list++;
  }
  dynamic_parser_closure* dpc = (dynamic_parser_closure *) calloc(1, sizeof(dynamic_parser_closure));
  dpc->tag = TYPE_FUNC_PTR;
  dpc->dp_ptr = oneof_dp;
  parser* p = parser_new(sc, dpc);
  free(sc);
  return p;
}

parser_dp_return oneof_dp(dynamic_parser_closure* unused, input_t* in) {
  //we know that the parse always succeed since it has passed the static parser
  parser_dp_return ret;
  ret.obj = char_to_ptr(in->input[in->cursor]);
  ret.i = input_next(in);
  return ret;
}

parser* parser_new(static_context* sc, dynamic_parser_closure* dpc) {
  parser* p = (parser *)calloc(1, sizeof(parser));
  p->status = PARSER_NORMAL;
  p->static_context = static_context_copy(sc);
  p->dpc = dpc;
  return p;
}

parser* parser_copy(parser* p) {
  return parser_new(p->static_context, p->dpc);
}

void parser_delete(parser* p) {
  //TODO: implement ref count to delete static_context and 
  //list_delete(p->static_context);
  //free(p->dpc);
  free(p);
}


char* char_to_ptr(char a) {
  if(!char_to_ptr_mapping[a]) {
    char_to_ptr_mapping[a] = a;
  }
  return &char_to_ptr_mapping[a];
}

char ptr_to_char(char* a) {
  return *a;
}

int ptr_to_int(int* a) {
  return *a;
}

int* int_to_ptr(int a) {
  if(!int_to_ptr_mapping[a]) {
    int_to_ptr_mapping[a] = a;
  }
  return &int_to_ptr_mapping[a];
}
