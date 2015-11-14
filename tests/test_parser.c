#include "../parser.h"
#include "../input.h"
#include "tests.h"

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
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(test_parser1_dp, 0);
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
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(test_parser1_dp, 0);
  input_t i;
  const char* input_str = "";
  input_init(&i, input_str);
  parser* p = parser_new(sc, dpc);
  parser* many_p = many(p);
  parser_dp_return parse_result = parse(many_p, i);
  parser_delete(p);
  bool result = true;
  test_true(&result, parse_result.status == PARSER_NORMAL);
  test_true(&result, !input_cmp(&parse_result.i, &i));
  test_true(&result, list_peek_back(parse_result.obj) == NULL);
  list_delete(parse_result.obj);
  return result;
}

int main() {
  test_case("test_parser1", test_parser1);
  test_case("test_parser2", test_parser2);
  test_case("test_parser3", test_parser3);
  test_report();
  return 0;
}
