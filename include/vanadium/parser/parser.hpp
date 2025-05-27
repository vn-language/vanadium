#ifndef INCLUDE_PARSER_PARSER_HPP_
#define INCLUDE_PARSER_PARSER_HPP_

#include "vanadium/parser/ast.hpp"
#include "vanadium/parser/lexer.hpp"
#include <cstddef>
#include <vector>

namespace vanadium {
namespace parser {

typedef std::vector<NodeP> NodeList;

class NodeStream {
public:
  NodeStream(NodeList input_nodes) : nodes(input_nodes), current(0) {};
  NodeStream(NodeStream &&) = default;
  NodeStream(const NodeStream &) = default;
  NodeStream &operator=(NodeStream &&) = default;
  NodeStream &operator=(const NodeStream &) = default;
  ~NodeStream() = default;

  NodeP next();
  NodeP get();
  bool has_next();
  const size_t get_index();
  const NodeList get_nodes();

private:
  NodeList nodes;
  size_t current;
};

class Parser {
public:
  Parser(lexer::TokenStream ts) : ts(ts) {};
  Parser(Parser &&) = default;
  Parser(const Parser &) = default;
  Parser &operator=(Parser &&) = default;
  Parser &operator=(const Parser &) = default;
  ~Parser() = default;

  NodeStream parse();

private:
  lexer::TokenStream ts;

  /* Main */
  NodeP parse_start();

  /* Expressions */
  NodeP parse_expr(); // Pratt parsing

  /* Declarations */
  NodeP parse_decl();
  std::vector<lexer::Token> parse_modifiers();
  NodeP parse_vardecl(std::vector<lexer::Token> modfs);  // VarDecl
  NodeP parse_funcdecl(std::vector<lexer::Token> modfs); // FuncDecl
  NodeP parse_classdecl();                               // ClassDecl
  NodeP parse_structdecl();                              // StructDecl
};

} // namespace parser
} // namespace vanadium

#endif // INCLUDE_PARSER_PARSER_HPP_
