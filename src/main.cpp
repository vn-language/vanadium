#include <cstdio>
#include <iostream>
#include <string>

#include "vanadium/diagnostics/diagnostics.hpp"
#include "vanadium/parser/errors.hpp"
#include "vanadium/parser/lexer.hpp"
#include "vanadium/parser/parser.hpp"

using namespace vanadium;

int main(int argc, char *argv[]) {
  /* const std::string input = R"(*/
  /* static func main() {*/
  /*let loves_cxx = true;*/
  /*throw new Exception("I hate C++") unless loves_cxx;*/
  /*@@ throw new Exception("I hate C++") unless loves_cxx ifso println("I LOVE
   * C++");*/
  /* }*/
  /*)";*/
  const std::string input = R"(
  from "std/IO" include println;

  static func main() {
	let
  }
  )";

  try {
    lexer::TokenStream ts = lexer::tokenize(input);

    /*for (auto &tk : ts.get_tokens()) {*/
    /*  tk.display();*/
    /*}*/

    try {
      parser::Parser p(ts);
      parser::NodeStream ast = p.parse();

      for (auto &node : ast.get_nodes()) {
        std::cout << node->as_string() << std::endl;
      }
    } catch (const parser::ParseError &e) {
      diagnostics::Diagnostic diag(diagnostics::Severity::Error, "Parse error",
                                   e.what());
      diag.add_label(
          diagnostics::Label("input hardcoded into src/main.cpp!", "Source"));
      diag.print();
    }
  } catch (const parser::ParseError &e) {
    diagnostics::Diagnostic diag(diagnostics::Severity::Error, "Lex error",
                                 e.what());

    diag.add_label(
        diagnostics::Label("input hardcoded into src/main.cpp!", "Source"));
    diag.print();
  }

  return 0;
}
