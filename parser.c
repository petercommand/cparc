#include <stdbool.h>

static char char_to_ptr_mapping[256];
static int int_to_ptr_mapping[INT_MAX];

list* static_context_new() {
  return list_new_with_tag(ELEM_CHAR);
}

parser_dp_return dynamic_parser_closure_eval(dynamic_parser_closure* closure, parser* p, input* input) {
  switch(closure->tag) {
  case TYPE_FUNC_PTR:
    return closure->dp_ptr(p, input);
    break;
  case TYPE_CLOSURE:
    return closure->dp_ptr(closure, p, input);
    break;
  }

}

void parse(parser* p, token* i, bool can_fail) {
  if(p.status == PARSER_FAILED) {
    if(!can_fail) {
      fatal_error(PARSER_ERROR, p->error_msg);
    }
    return;
  }
  //NOT FINISHED  
}

parser* choice(parser* a, parser* b) {
  //choice operator
  bool allow_empty = false;
  list* new_static_context = static_context_new();
  if(a->allow_empty || b->allow_empty) {
    allow_empty = true;
  }
  list_append(new_static_context, a->static_context);
  list_append(new_static_context, b->static_context);
  dynamic_parser* dpc = choice_dpc(a->dpc, b->dpc);
  return parser_new(new_static_context, dpc, allow_empty);
}

dynamic_parser_closure* choice_dpc(dynamic_parser_closure* a, dynamic_parser_closure* b) {
  dynamic_parser_closure* dpc = (dynamic_parser_closure *)calloc(sizeof(dynamic_parser_closure));
  
  dpc->tag_t = TYPE_CLOSURE;
  dpc->two_ptr.ptr1 = a;
  dpc->two_ptr.ptr2 = b;
  dpc->dp_ptr = choice_dp;
  
  return dpc;
}

parser_dp_return choice_dp(dynamic_parser_closure* ctx, parser* p, input* input) {

  parser_dp_return result1 = dynamic_parser_closure_eval(ctx->two_ptr->ptr1, p, input);
  if(result1->obj) {//successful parse
    return result1;
  }
  else if(p->can_fail) {//failed parse, try second parser
    p->status = PARSER_NORMAL;
    parser_dp_return result2 = dynamic_parser_closure_eval(ctx->two_ptr->ptr2, p, input);
    return result2;
  }
  else {
    return result1;
  }
}

parser* symbol(char sym) {
  list* static_context = static_context_new();
  char* elem = char_to_ptr(*list);
  //NOT FINISHED
}

parser* oneof(char* list) {
  //create a new parser that consume exactly one char that is in the list
  list* static_context = static_context_new();
  char* elem;
  while(*list) {
    elem = char_to_ptr(*list);
    list_push_back(static_context, elem);
    list++;
  }
  dynamic_parser* dp = oneof_dp;
  dynamic_parser_closure dpc;
  dpc.tag = TYPE_FUNC_PTR;
  dpc.ptr = dp;
  parser* p = parser_new(static_context, dpc, false);
  free(static_context);
  return p;
}

parser_dp_return oneof_dp(parser* p, input* in) {
  //we know that the parse always succeed since it has passed the static parser
  parser_dp_return ret;
  ret.obj = char_to_ptr(in->input[in->cursor]);
  ret.i = input_next(in);
  return ret;
}

parser* parser_new(list* static_context, dynamic_parser_closure dpc, bool allow_empty) {
  parser* p = (parser *)calloc(sizeof(parser));
  p.status = PARSER_NORMAL;
  p.sc = list_copy(static_context);
  p.dpc = dpc;
  p.allow_empty = allow_empty;
  return p;
}

void parser_delete(parser* p) {
  list_delete(p->static_context);
  free(p);
}


void parser_init() {
  for(int i = 0;i < 256;i++) {
    char_to_ptr_mapping[i] = i;
  }
}

char* char_to_ptr(char a) {
  if(!char_to_ptr_mapping[a]) {
    char_to_ptr_mapping[a] = a;
  }
  return &char_to_ptr_mapping[a];
}

int* int_to_ptr(int a) {
  if(!int_to_ptr_mapping[a]) {
    int_to_ptr_mapping[a] = a;
  }
  return &int_to_ptr_mapping[a];
}
