#include <stdbool.h>

static char char_to_ptr_mapping[256];
static int int_to_ptr_mapping[INT_MAX];

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
    new_sc->allow_empty = allow_empty;
  }
  return new_sc;
}

static_context* static_context_from_list(list* list, bool allow_empty) {
  static_context* sc = static_context_new();
  if(new_sc) {
    sc->list = list_copy(list);
    sc->allow_empty = allow_empty;
  }
  return sc;
}

bool match_range_criteria(void* elem, input* i) {

}

bool match_char_criteria(char* elem, input* i) {
  if(input_peek(i) == ptr_to_char(elem)) {
    return true;
  }
}

bool match_str_criteria(char* elem, input* i) {
  
}

bool match_static_criteria(STATIC_PARSER_ELEM_TYPE tag, void* elem, input* i) {
  //this function checks whether the input matches the given static criteria
  switch(li->tag) {
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

bool static_match(static_context* sc, input* i) {
  if(input_peek(i) == '\0' && sc->allow_empty) {
    return true;
  }
  if(input_peek(i) == '\0' && !sc->allow_empty) {
    return false;
  }
  list_item* head = sc->list->head;
  while(head) {
    if(static_match_criteria(head, i)) {
      return true;
    }
  }
  return false;
}

parser_dp_return dynamic_parser_closure_eval(dynamic_parser_closure* closure, parser* p, input* input) {
  switch(closure->tag) {
  case TYPE_FUNC_PTR:
    return closure->dp_ptr(NULL, p, input);
    break;
  case TYPE_CLOSURE:
    return closure->dp_ptr(closure, p, input);
    break;
  }

}
void parse(parser* p, input* i, bool can_fail) {
  if(p.status == PARSER_FAILED) {
    if(!can_fail) {
      fatal_error(PARSER_ERROR, p->error_msg);
    }
    return;
  }
  if(static_match(p->sc, i)) {
    parser_dp_return dynamic_parser_closure_eval
  }
  //NOT FINISHED  
  
}

parser* concat(parser* a, parser* b) {
  bool allow_empty = a->allow_empty && b->allow_empty;
  static_context* sc = static_context_new();
  }

parser* choice(parser* a, parser* b) {
  //choice operator
  static_context* sc = static_context_new();

  static_context_append(sc, a->static_context);
  static_context_append(sc, b->static_context);
  sc->allow_empty = a->allow_empty || b->allow_empty;
  dynamic_parser* dpc = choice_dpc(a, b);
  return parser_new(new_static_context, dpc);
}

dynamic_parser_closure* choice_dpc(list* static_a, dynamic_parser_closure* dp_a, list* static_b, dynamic_parser_closure* b) {
  dynamic_parser_closure* dpc = (dynamic_parser_closure *)calloc(sizeof(dynamic_parser_closure));
  dpc->tag_t = TYPE_CLOSURE;
  dpc->ctx->static1 = static_a;
  dpc->ctx->dpc1 = dp_a;
  dpc->ctx->static2 = static_b;
  dpc->ctx->dpc2 = dp_b;
  return dpc;
}

parser_dp_return choice_dp(dynamic_parser_closure* dpc, parser* p, input* input) {
  if(static_match(p, input)) {
    parser_dp_return result1 = dynamic_parser_closure_eval(dpc->ctx->dpc1, p, input);
    if(result1->obj) {//successful parse
      return result1;
    }
    else if(p->can_fail) {//failed parse, try second parser
      p->status = PARSER_NORMAL;
      parser_dp_return result2 = dynamic_parser_closure_eval(dpc->ctx->dpc2, p, input);
      return result2;
    }
    else {
      return result1;
    }
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
    list_push_back_with_tag(static_context, elem, ELEM_CHAR);
    list++;
  }
  dynamic_parser* dp = oneof_dp;
  dynamic_parser_closure dpc;
  dpc.tag = TYPE_FUNC_PTR;
  dpc.ptr = dp;
  parser* p = parser_new(static_context, dpc);
  free(static_context);
  return p;
}

parser_dp_return oneof_dp(dynamic_parser_closure* unused, parser* p, input* in) {
  //we know that the parse always succeed since it has passed the static parser
  parser_dp_return ret;
  ret.obj = char_to_ptr(in->input[in->cursor]);
  ret.i = input_next(in);
  return ret;
}

parser* parser_new(list* static_context, dynamic_parser_closure dpc) {
  parser* p = (parser *)calloc(sizeof(parser));
  p.status = PARSER_NORMAL;
  p.sc = static_context_copy(static_context);
  p.dpc = dpc;
  return p;
}

void parser_delete(parser* p) {
  list_delete(p->static_context);
  free(p->dpc);
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

int int_to_ptr(int* a) {
  return *a;
}

int* int_to_ptr(int a) {
  if(!int_to_ptr_mapping[a]) {
    int_to_ptr_mapping[a] = a;
  }
  return &int_to_ptr_mapping[a];
}
