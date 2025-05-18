#include <cstdio>

#include "vanadium/parser/lexer.hpp"

int main(int argc, char *argv[]) {
  TokenStream ts = tokenize("500 1341 5.15 \n \"Hello, World!\" identifier");
  for (const auto token : ts.get_tokens()) {
    token.display();
  }

  /*while (ts.has_next()) {*/
  /*  ts.next().display();*/
  /*}*/
  return 0;
}
