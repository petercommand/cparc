#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#include "parser.h"
#include "list.h"
#include "input.h"

static char char_to_ptr_mapping[256];
static int* int_to_ptr_mapping = NULL;

parser_dp_return oneof_dp(dynamic_parser_closure* unused, input_t in);


parser_dp_return choice_dp(dynamic_parser_closure* dpc, input_t input);


parser* try_parser(parser* p) {
  //does not produce a new parser, instead, try_parser transforms existing parser into a parser that can fail
  p->sc->can_fail = true;
  return p;
}


static_context* static_context_new() {
  static_context* sc = (static_context *)malloc(sizeof(static_context));
  if(sc) {
    sc->list = list_new();
    sc->allow_empty = false;
    sc->ref_count = 0;
  }
  return sc;
}

void static_context_delete(static_context* sc) {
  if(sc && sc->ref_count > 0) {
    sc->ref_count--;
    if(!sc->ref_count) {
      list_item* head = sc->list->head;
      while(head) {
	if(head->tag == ELEM_RANGE) {
	  range_criteria_delete(head->item);
	}
	head = head->next;
      }
      list_delete(sc->list);
      free(sc);
    }
  }
}

closure_ctx* closure_ctx_new(static_context* sc, dynamic_parser_closure* dpc) {
  closure_ctx* ctx = (closure_ctx *) malloc(sizeof(closure_ctx));
  ctx->sc = sc;
  sc->ref_count++;
  ctx->dpc = dpc;
  dpc->ref_count++;
  ctx->ref_count = 0;
  return ctx;
}

void closure_ctx_delete(closure_ctx* ctx) {
  if(ctx) {
    ctx->ref_count--;
    if(!ctx->ref_count) {
      static_context_delete(ctx->sc);
      dynamic_parser_closure_delete(ctx->dpc);
      free(ctx); 
    }
  }
}

dynamic_parser_closure* dynamic_parser_closure_new_p(closure_ctx** ctxes, dynamic_parser dp_ptr) {
  dynamic_parser_closure* dpc = (dynamic_parser_closure *) calloc(1, sizeof(dynamic_parser_closure));
  dpc->ctxes = ctxes;
  dpc->dp_ptr = dp_ptr;
  dpc->ref_count = 0;
  return dpc;
}

void dynamic_parser_closure_delete(dynamic_parser_closure* dpc) {
  if(dpc && dpc->ref_count > 0) {
    dpc->ref_count--;
    if(!dpc->ref_count) {
      for(int i = 0;dpc->ctxes[i] != 0;i++) {
	closure_ctx_delete(dpc->ctxes[i]);
      }
      free(dpc->ctxes);
      free(dpc);
    }
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

range_criteria* range_criteria_new() {
  return (range_criteria *)calloc(1, sizeof(range_criteria));
}

void range_criteria_delete(range_criteria* r) {
  free(r);
}

bool match_range_criteria(range_criteria* elem, const input_t* i) {
  list_item* range = elem->range->head;
  for(int j = 0;j < elem->num;j++) {
    char current = i->input[i->cursor + j];
    if(current != '\0') {
      range_item* ri = range->item;
      if(!((current >= ri->lower_bound) && (current <= ri->upper_bound))) {
	return false;
      }
      range = range->next;
    }
    else {
      return false;
    }
  }
  return true;
}

bool match_char_criteria(char* elem, const input_t* i) {
  if(input_peek(i) == ptr_to_char(elem)) {
    return true;
  }
  return false;
}

bool match_str_criteria(char* elem, const input_t* i) {
  size_t j = 0;
  while(*elem != '\0') {
    if(i->input[i->cursor + j] == '\0') {
      return false;
    }
    if(*elem != i->input[i->cursor + j]) {
      return false;
    }
    elem++;
    j++;
  }
  return true;
}

bool match_static_criteria(tag_t tag, void* elem, const input_t* i) {
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

bool static_match(static_context* sc, const input_t* i) {
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
    head = head->next;
  }
  return false;
}

  

parser_dp_return dynamic_parser_closure_eval(dynamic_parser_closure* closure, input_t input) {
  return closure->dp_ptr(closure, input);
}
parser_dp_return parse(parser* p, input_t i) {
  return parse1(p->sc, p->dpc, i);
}
parser_dp_return parse1(static_context* sc, dynamic_parser_closure* dpc, input_t i) {
  if(static_match(sc, &i) || sc->allow_empty) {
    return dynamic_parser_closure_eval(dpc, i);
  }
  else {
    parser_dp_return failed;
    failed.status = PARSER_FAILED;
    failed.obj = NULL;
    failed.i = i;
    return failed;
  }
}

void dynamic_parser_closure_replace_context(dynamic_parser_closure* dpc, size_t n, closure_ctx* ctx) {
  dpc->ctxes[n] = ctx;
  ctx->ref_count++;
}

dynamic_parser_closure* dynamic_parser_closure_new(dynamic_parser dp, size_t size, ...) {
  va_list argp;
  va_start(argp, size);
  closure_ctx* ctx;
  closure_ctx** ctxes = (closure_ctx **) calloc(size + 1, sizeof(closure_ctx *));
  size_t i = 0;
  while(i < size) {
    ctx = va_arg(argp, closure_ctx *);
    if(ctx) {
      ctx->ref_count++;
    }
    ctxes[i] = ctx;
    i++;
  }
  ctxes[i] = 0;
  va_end(argp);
  return dynamic_parser_closure_new_p(ctxes, dp);
}

parser* choice(parser* a, parser* b) {
  //choice operator
  static_context* sc = static_context_new();

  list_append(sc->list, a->sc->list);
  list_append(sc->list, b->sc->list);
  sc->allow_empty = a->sc->allow_empty || b->sc->allow_empty;

  closure_ctx* ctx1 = closure_ctx_new(a->sc, a->dpc);
  closure_ctx* ctx2 = closure_ctx_new(b->sc, b->dpc);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(choice_dp, 2, ctx1, ctx2);
  return parser_new(sc, dpc);
}

parser_dp_return then_dp(dynamic_parser_closure* dpc, input_t input) {
  void* obj1;
  void (*discard_obj1_callback)(void *);
  parser_dp_return result = dynamic_parser_closure_eval(dpc->ctxes[0]->dpc, input);
  if(result.status == PARSER_NORMAL) {
    obj1 = result.obj;
    discard_obj1_callback = result.discard_obj_callback;
    result = parse1(dpc->ctxes[1]->sc, dpc->ctxes[1]->dpc, result.i);
    if(result.status == PARSER_NORMAL) {
      list* l = list_new();
      list_push_back(l, obj1);
      list_push_back(l, result.obj);
      result.obj = l;
      result.discard_obj_callback = list_delete;
      return result;
    }
    else {
      if(discard_obj1_callback) {
	discard_obj1_callback(obj1);
      }
    }
  }
  result.status = PARSER_FAILED;
  return result;
}

parser* then(parser* a, parser* b) {
  //then operator
  static_context* sc = static_context_new();
  list_append(sc->list, a->sc->list);
  list_append(sc->list, b->sc->list);
  sc->allow_empty = a->sc->allow_empty && b->sc->allow_empty;
  closure_ctx* ctx1 = closure_ctx_new(a->sc, a->dpc);
  closure_ctx* ctx2 = closure_ctx_new(b->sc, b->dpc);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(then_dp, 2, ctx1, ctx2);
  return parser_new(sc, dpc);
}
    

parser_dp_return choice_dp(dynamic_parser_closure* dpc, input_t input) {
  if(input_peek(&input) == '\0') {//empty input
    if(dpc->ctxes[0]->sc->allow_empty) {
      return parse1(dpc->ctxes[0]->sc, dpc->ctxes[0]->dpc, input);
    }
    else {
      return parse1(dpc->ctxes[1]->sc, dpc->ctxes[1]->dpc, input);
    }
  }
  else {//nonempty input
    parser_dp_return result1 = parse1(dpc->ctxes[0]->sc, dpc->ctxes[0]->dpc, input);
    if(result1.status == PARSER_NORMAL) {//successful parse
      return result1;
    }
    else {//failed parse
      if(dpc->ctxes[0]->sc->can_fail) {//failed parse, try second parser
	//we are discarding the first result, so we explicitly deallocate its result if discard_obj_callback is set
	if(result1.discard_obj_callback && result1.obj) {
	  result1.discard_obj_callback(result1.obj);
	}
	return parse1(dpc->ctxes[1]->sc, dpc->ctxes[1]->dpc, input);
      }
      else {
	return result1;
      }
    }
  }
}

parser_dp_return many_dp(dynamic_parser_closure* dpc, input_t in) {
  //user should explicitly deallocate this list
  parser_dp_return ret;
  ret.obj = NULL;
  ret.status = PARSER_NORMAL;
  ret.i = in;
  ret.discard_obj_callback = NULL;
  parser_dp_return last_ret;
  list* l = list_new();
  while(ret.status == PARSER_NORMAL) {
    last_ret = ret;
    ret = parse1(dpc->ctxes[0]->sc, dpc->ctxes[0]->dpc, ret.i);
    if(ret.status == PARSER_NORMAL) {
      list_push_back(l, ret.obj);
    }
  }
  last_ret.discard_obj_callback = list_delete;
  last_ret.obj = l;
  return last_ret;
}

parser* many(parser* p) {
  static_context* sc = static_context_copy(p->sc);
  sc->allow_empty = true;
  closure_ctx* ctx1 = closure_ctx_new(p->sc, p->dpc);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(many_dp, 1, ctx1);
  return parser_new(sc, dpc);
}

parser_dp_return many1_dp(dynamic_parser_closure* dpc, input_t in) {
  parser_dp_return ret = many_dp(dpc, in);
  if(!input_cmp(&ret.i, &in)) {
    ret.status = PARSER_FAILED;
  }
  return ret;
}

parser* many1(parser* p) {
  closure_ctx* ctx1 = closure_ctx_new(p->sc, p->dpc);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(many1_dp, 1, ctx1);
  return parser_new(p->sc, dpc);
}

parser_dp_return symbol_dp(dynamic_parser_closure* dpc, input_t in) {
  parser_dp_return ret;
  ret.i = input_next(in);
  ret.status = PARSER_NORMAL;
  ret.obj = char_to_ptr(input_peek(&in));
  ret.discard_obj_callback = NULL;
  return ret;
}

parser* symbol(char sym) {
  static_context* sc = static_context_new();
  char* elem = char_to_ptr(sym);
  static_context_add(sc, elem, ELEM_CHAR);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(symbol_dp, 0);
  return parser_new(sc, dpc);
}

parser_dp_return parser_chain_dp(dynamic_parser_closure* dpc, input_t in) {

}

parser* parser_chain(list* parsers, size_t num, input_t input) {
  list_item* head = parsers->head;
  closure_ctx* ctx;
  bool allow_empty = true;
  static_context* sc = static_context_new();
  bool initial = true;
  closure_ctx** ctxes = calloc(num + 2, sizeof(closure_ctx *));
  int i = 1;//preserve i = 0 for self closure
  while(head) {
    parser* current = head->item;
    if(!current->sc->allow_empty) {
      if(allow_empty) {
	//first non_empty parser
	static_context_append(sc, current->sc);
	initial = false;
      }
      allow_empty = false;
    }
    if(initial) {
      static_context_append(sc, current->sc);
    }
    
    ctx = closure_ctx_new(current->sc, current->dpc);
    ctxes[i] = ctx;
    i++;
    ctx->ref_count++;
    head = head->next;
  }
  dynamic_parser_closure* dpc = dynamic_parser_closure_new_p(ctxe, parser_chain_dp);
  return parser_new(sc, dpc);
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
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(oneof_dp, 0);
  parser* p = parser_new(sc, dpc);
  return p;
}

parser_dp_return eof_dp(dynamic_parser_closure* dpc, input_t in) {
  parser_dp_return dp_ret;
  dp_ret.i = input_next(in);
  dp_ret.status = PARSER_NORMAL;
  dp_ret.discard_obj_callback = NULL;
  dp_ret.obj = NULL;
  return dp_ret;
}

parser* eof() {
  static_context* sc = static_context_new();
  static_context_add(sc, char_to_ptr('\0'), ELEM_CHAR);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(eof_dp, 0);
  return parser_new(sc, dpc);
}
parser_dp_return oneof_dp(dynamic_parser_closure* unused, input_t in) {
  //we know that the parse always succeed since it has passed the static parser
  parser_dp_return ret;
  ret.obj = char_to_ptr(input_peek(&in));
  ret.i = input_next(in);
  ret.status = PARSER_NORMAL;
  ret.discard_obj_callback = NULL;
  return ret;
}

//do not change static_context or dyamic_parser_closure after parser_new since it shares them btw parsers
parser* parser_new(static_context* sc, dynamic_parser_closure* dpc) {
  parser* p = (parser *)calloc(1, sizeof(parser));
  p->sc = sc;
  p->sc->ref_count++;
  p->dpc = dpc;
  p->dpc->ref_count++;
  return p;
}

parser* parser_copy(parser* p) {
  return parser_new(p->sc, p->dpc);
}

void parser_delete(parser* p) {
  static_context_delete(p->sc);
  dynamic_parser_closure_delete(p->dpc);
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
  return ((int)a) - 1;
}

int* int_to_ptr(int a) {
  return (int *)(a + 1);
}
