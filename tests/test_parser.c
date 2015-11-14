#include "../parser.h"

pserser_dp_return test_parser1_dp(dynamic_parser_closure* ctx, input_t input) {
  parser_dp_return dp_ret;
  dp_ret.obj = NULL;
  dp_ret.status = PARSER_NORMAL;
  dp_ret.i = ;
}
bool test_parser1() {
  static_context* sc = static_context_new();
  static_context_add(sc, "input", ELEM_STR);
  dynamic_parser_closure* dpc = dynamic_parser_closure_new(test_parser1_dp, 0);
  input i;
  const char* input_str = "input";
  input_init(i, input_str);
  parser* p = parser_new(sc, dpc);
  bool result = parser(p, i);
  parser_delete(p);
}

int main() {
  test_case("test_parser1", test_parser1);
  return 0;
}
