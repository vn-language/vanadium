#include <cstdio>
/*#include <iostream>*/
#include <string>

/*#include "vanadium/parser/ast.hpp"*/
#include "vanadium/parser/lexer.hpp"
/*#include "vanadium/parser/parser.hpp"*/

using namespace vanadium;

int main(int argc, char *argv[]) {
  const std::string input = R"(
  from "std/io" include println;

  static func main() {
	const x = 2 + 3.3 - 8.;
	let no_errors = true;
	try {
	  let y = x as string;
	} catch {|e| println(e); no_errors = false};

	println("No errors!") unless not no_errors ifso println("Errors!");
  })";
  lexer::TokenStream ts = lexer::tokenize(input);

  while (ts.has_next()) {
    ts.next().display();
  }

  /*parser::Parser p(ts);*/
  /*parser::AST ast = p.parse();*/
  /**/
  /*for (auto node : ast.iter()) {*/
  /*  std::cout << node->as_string() << std::endl;*/
  /*}*/

  return 0;
}
