#include "../parser.h"
#include "../input.h"
#include "tests.h"
#include <string.h>

parser_dp_return test_parser1_dp(dynamic_parser_closure* ctx, input_t input) {
  parser_dp_return dp_ret;
  dp_ret.obj = NULL;
  dp_ret.status = PARSER_NORMAL;
  dp_ret.i = input_forward(input, 5);
  return dp_ret;
}
bool test_parser1() {
  static_context* sc = static_context_new();
  static_context_add(sc, "input", ELEM_STR);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(test_parser1_dp, 0);
  input_t i;
  const char* input_str = "input";
  input_init(&i, input_str);
  parser* p = parser_new(sc, dpc);
  parser_dp_return parse_result = parse(p, i);
  parser_delete(p);
  bool result = true;
  test_true(&result, parse_result.status == PARSER_NORMAL);
  input_t expected_i = input_forward(i, 5);
  test_true(&result, !input_cmp(&parse_result.i, &expected_i));
  test_true(&result, parse_result.obj == NULL);
  return result;
}

parser_dp_return test_parser2_dp(dynamic_parser_closure* ctx, input_t input) {
  parser_dp_return dp_ret;
  dp_ret.obj = NULL;
  dp_ret.status = PARSER_NORMAL;
  dp_ret.i = input_forward(input, 5);
  return dp_ret;
}
bool test_parser2() {
  static_context* sc = static_context_new();
  static_context_add(sc, "input", ELEM_STR);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(test_parser2_dp, 0);
  input_t i;
  const char* input_str = "input()";
  input_init(&i, input_str);
  parser* p = parser_new(sc, dpc);
  parser_dp_return parse_result = parse(p, i);
  parser_delete(p);
  bool result = true;
  test_true(&result, parse_result.status == PARSER_NORMAL);
  input_t expected_i = input_forward(i, 5);
  test_true(&result, !input_cmp(&parse_result.i, &expected_i));
  test_true(&result, parse_result.obj == NULL);
  return result;
}

parser_dp_return test_parser3_dp(dynamic_parser_closure* ctx, input_t input) {
  parser_dp_return dp_ret;
  dp_ret.obj = NULL;
  dp_ret.status = PARSER_NORMAL;
  dp_ret.i = input_forward(input, 2);
  return dp_ret;
}
bool test_parser3() {
  static_context* sc = static_context_new();
  static_context_add(sc, "()", ELEM_STR);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(test_parser3_dp, 0);
  input_t i;
  const char* input_str = "";
  input_init(&i, input_str);
  parser* p = parser_new(sc, dpc);
  parser* many_p = many(p);
  parser_dp_return parse_result = parse(many_p, i);
  parser_delete(p);
  parser_delete(many_p);
  bool result = true;
  test_true(&result, parse_result.status == PARSER_NORMAL);
  test_true(&result, !input_cmp(&parse_result.i, &i));
  test_true(&result, list_peek_back(parse_result.obj) == NULL);
  list_delete(parse_result.obj);
  return result;
}
parser_dp_return test_parser4_dp(dynamic_parser_closure* ctx, input_t input) {
  parser_dp_return dp_ret;
  dp_ret.obj = "()";
  dp_ret.status = PARSER_NORMAL;
  dp_ret.i = input_forward(input, 2);
  return dp_ret;
}
bool test_parser4() {
  static_context* sc = static_context_new();
  static_context_add(sc, "()", ELEM_STR);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(test_parser4_dp, 0);
  input_t i;
  const char* input_str = "()()()()";
  input_init(&i, input_str);
  parser* p = parser_new(sc, dpc);
  parser* many_p = many(p);
  parser_dp_return parse_result = parse(many_p, i);
  parser_delete(p);
  parser_delete(many_p);
  bool result = true;
  test_true(&result, parse_result.status == PARSER_NORMAL);
  input_t expected_i = input_forward(i, 8);
  test_true(&result, !input_cmp(&parse_result.i, &expected_i));
  test_true(&result, !strcmp(list_peek_back(parse_result.obj), "()"));
  list_delete(parse_result.obj);
  return result;
}

parser_dp_return test_parser5_dp(dynamic_parser_closure* ctx, input_t input) {
  bool match = match_str_criteria("()", &input);
  parser_dp_return dp_ret;
  if(match) {
    dp_ret.obj = "()";
    dp_ret.status = PARSER_NORMAL;
    dp_ret.i = input_forward(input, 2);
  }
  else {
    dp_ret.status = PARSER_FAILED;
    dp_ret.obj = NULL;
    dp_ret.i = input_forward(input, 1);
  }
  return dp_ret;
}
bool test_parser5() {
  static_context* sc = static_context_new();
  static_context_add(sc, char_to_ptr('('), ELEM_CHAR);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(test_parser5_dp, 0);
  input_t i;
  const char* input_str = "(";
  input_init(&i, input_str);
  parser* p = parser_new(sc, dpc);
  parser* many1_p = many1(p);
  parser_dp_return parse_result = parse(many1_p, i);
  parser_delete(p);
  parser_delete(many1_p);
  bool result = true;
  test_true(&result, parse_result.status == PARSER_FAILED);
  list_delete(parse_result.obj);
  return result;
}

parser_dp_return test_parser6_if_dp(dynamic_parser_closure* ctx, input_t input) {
  parser_dp_return dp_ret;
  dp_ret.status = PARSER_NORMAL;
  dp_ret.obj = NULL;
  dp_ret.i = input_forward(input, 2);
  return dp_ret;
}

parser_dp_return test_parser6_iff_dp(dynamic_parser_closure* ctx, input_t input) {
  parser_dp_return dp_ret;
  dp_ret.status = PARSER_NORMAL;
  dp_ret.obj = NULL;
  dp_ret.i = input_forward(input, 3);
  return dp_ret;
}

bool test_parser6() {
  //here we present two cases that is not mutually exclusive to test the parser
  static_context* if_sc = static_context_new();
  static_context_add(if_sc, "if", ELEM_STR);
  dynamic_parser_closure* if_dpc = dynamic_parser_closure_new(test_parser6_if_dp, 0);

  static_context* iff_sc = static_context_new();
  static_context_add(iff_sc, "iff", ELEM_STR);
  dynamic_parser_closure* iff_dpc = dynamic_parser_closure_new(test_parser6_iff_dp, 0);
  

  input_t i;
  const char* input_str = "iff";
  input_init(&i, input_str);

  
  parser* if_p = parser_new(if_sc, if_dpc);
  try_parser(if_p);
  parser* iff_p = parser_new(iff_sc, iff_dpc);
  parser* choice_p = choice(if_p, iff_p);
  parser_delete(if_p);
  parser_delete(iff_p);
  parser_dp_return parse_result = parse(choice_p, i);
  parser_delete(choice_p);
  bool result = true;
  test_true(&result, parse_result.status == PARSER_NORMAL);
  list_delete(parse_result.obj);
  return result;
}

int main() {
  test_case("test_parser1", test_parser1);
  test_case("test_parser2", test_parser2);
  test_case("test_parser3", test_parser3);
  test_case("test_parser4", test_parser4);
  test_case("test_parser5", test_parser5);
  test_case("test_parser6", test_parser6);
  test_report();
  if(total_error == 0) {
    return 0;
  }
  return 1;
}
