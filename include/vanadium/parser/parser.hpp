#ifndef INCLUDE_PARSER_PARSER_HPP_
#define INCLUDE_PARSER_PARSER_HPP_

#include "vanadium/parser/ast.hpp"
#include "vanadium/parser/lexer.hpp"
#include <cstddef>
#include <utility>
#include <vector>

namespace vanadium {
namespace parser {

enum Precedence {
  PREC_LOWEST = 0,
  PREC_ASSIGNMENT = 1,
  PREC_TERM = 2,
  PREC_FACTOR = 3,
  PREC_PREFIX = 4,
  PREC_CALL = 5,
};

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
  NodeP parse_expr();
  NodeP parse_block();
  NodeP parse_type();
  NodeP parse_new();
  NodeP parse_throw();

  /* Pratt */
  NodeP pratt(int precedence);
  NodeP parse_prefix();
  NodeP parse_infix(NodeP left, Precedence precedence);

  /* Declarations */
  NodeP parse_decl();
  NodeP parse_vardecl(std::vector<lexer::TokenType> modfs);
  NodeP parse_funcdecl(std::vector<lexer::TokenType> modfs);
  NodeP parse_classdecl();
  NodeP parse_structdecl();

  /* Misc. */
  NodeP parse_include();
  std::vector<lexer::TokenType> parse_modifiers();
  std::vector<std::pair<std::string, NodeP>> parse_parameters();
};

} // namespace parser
} // namespace vanadium

#endif // INCLUDE_PARSER_PARSER_HPP_
