#include "vanadium/parser/parser.hpp"
#include "vanadium/parser/ast.hpp"
#include "vanadium/parser/errors.hpp"
#include "vanadium/parser/lexer.hpp"
#include "vanadium/parser/pmacros.hpp"
#include "vanadium/util_macros.hpp"
#include <algorithm>
#include <cstddef>
#include <vector>

using TType = vanadium::lexer::TokenType;

namespace vanadium {
namespace parser {

/* NodeStream methods */
NodeP NodeStream::next() { return nodes.at(current++); }
NodeP NodeStream::get() { return nodes.at(current); }

bool NodeStream::has_next() { return nodes.size() > current + 1; }

const size_t NodeStream::get_index() { return current; }
const NodeList NodeStream::get_nodes() { return nodes; }

/* Main parser logic */
NodeStream Parser::parse() {
  std::vector<NodeP> nodes;
  while (ts.has_next()) {
    nodes.push_back(parse_start());
  }
  return NodeStream(nodes);
}

std::vector<lexer::Token> Parser::parse_modifiers() {
  std::vector<lexer::Token> modifiers;

  TType t = ts.get().kind;
  while (t == TType::Static || t == TType::Sealed || t == TType::Export) {
    t = ts.get().kind;
    modifiers.push_back(ts.get());
    ts.next();
  }

  return modifiers;
}

#define UNEXPECTED_SEALED()                                                    \
  for (const auto modf : modifiers) {                                          \
    if (MATCH(modf, TType::Sealed)) {                                          \
      throw UnexpectedToken(modf, "not a class declaration");                  \
    }                                                                          \
  }

NodeP Parser::parse_start() {
  auto modifiers = parse_modifiers();

  const lexer::Token &next = ts.get();

  switch (next.kind) {
  case TType::Let:
  case TType::Const:
    UNEXPECTED_SEALED()

    TODO("Variable declarations");
    return parse_vardecl(modifiers);

  case TType::Func:
    UNEXPECTED_SEALED()

    TODO("Function declarations");
    return parse_funcdecl(modifiers);

  case TType::Class:
    TODO("Class declarations");
    return parse_classdecl();

  case TType::Struct:
    UNEXPECTED_SEALED()

    TODO("Structure declarations");
    return parse_structdecl();

  default:
    throw UnexpectedToken(next, "at start of declaration");
  }
}

NodeP parse_expr() { TODO("Pratt parsing"); }

} // namespace parser
} // namespace vanadium
