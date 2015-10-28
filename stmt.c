#include "parser.h"

parser* stmt_parser;

void parse_input(token* token) {
  init();
  while(token) {
    parse_stmt(token);

  }

  finalize();
}

static void init() {
  stmt_parser = create_parser(
