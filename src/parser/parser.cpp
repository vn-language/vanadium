#include "vanadium/parser/parser.hpp"
#include "vanadium/parser/ast.hpp"
#include "vanadium/parser/errors.hpp"
#include "vanadium/parser/lexer.hpp"
#include "vanadium/parser/pmacros.hpp"
#include "vanadium/util_macros.hpp"
#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

using TType = vanadium::lexer::TokenType;

#define SHOW_RULE_STARTS

#ifdef SHOW_RULE_STARTS
#define ST_RULE(NAME) std::cout << "Starting rule '" NAME "'" << std::endl
#else
#define ST_RULE(NAME)                                                          \
  do {                                                                         \
  } while (false)
#endif

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
    if (MATCH(ts.get(), TType::EOI)) {
      break;
    }
    nodes.push_back(parse_start());
    if (MATCH(ts.get(), TType::EOS)) {
      ts.next();
    }
  }
  return NodeStream(nodes);
}

std::vector<lexer::TokenType> Parser::parse_modifiers() {
  ST_RULE("parse_modifiers");

  std::vector<lexer::TokenType> modifiers;

  if (MATCH(ts.get(), TType::Const) || MATCH(ts.get(), TType::Let)) {
    modifiers = {};
    return modifiers;
  }

  TType t = ts.get().kind;
  while (t == TType::Static || t == TType::Sealed || t == TType::Export) {
    modifiers.push_back(t);
    ts.next();
    t = ts.get().kind;
  }

  return modifiers;
}

#define UNEXPECTED_SEALED()                                                    \
  for (const auto &modf : modifiers) {                                         \
    if (modf == TType::Sealed) {                                               \
      throw UnexpectedToken(lexer::Token(TType::Sealed, "sealed"),             \
                            "at a non-class declaration");                     \
    }                                                                          \
  }

NodeP Parser::parse_start() {
  ST_RULE("parse_start");

  if (MATCH(ts.get(), TType::From) || MATCH(ts.get(), TType::Include)) {
    return parse_include();
  }

  if (MATCH(ts.get(), TType::Throw)) {
    return parse_throw();
  }

  try {
    return to_nodep(ImplicitReturnNode(parse_expr()));
  } catch (const std::exception &) {
  }

  auto modifiers = parse_modifiers();

  const lexer::Token &tk = ts.get();

  if (MATCH(tk, TType::EOI)) {
    return to_nodep(EOINode());
  }

  switch (tk.kind) {
  case TType::Let:
  case TType::Const:
    UNEXPECTED_SEALED();
    return parse_vardecl(modifiers);

  case TType::Func:
    UNEXPECTED_SEALED();
    return parse_funcdecl(modifiers);

  case TType::Class:
    return parse_classdecl();

  case TType::Struct:
    UNEXPECTED_SEALED();
    return parse_structdecl();

  default:
    throw UnexpectedToken(tk, "at start of declaration");
  }
}

/* Declarations */

NodeP Parser::parse_vardecl(std::vector<lexer::TokenType> modfs) {
  ST_RULE("parse_vardecl");

  if (NMATCH(ts.get(), TType::Let) && NMATCH(ts.get(), TType::Const)) {
    throw ExpectedOneOfTokens({TType::Let, TType::Const}, ts.get());
  }

  bool is_const = false;
  if (MATCH(ts.get(), TType::Const)) {
    is_const = true;
  }

  bool is_static = false;
  if (VEC_HAS(modfs, TType::Static)) {
    is_static = true;
  }

  if (NMATCH(ts.next(), TType::Ident)) {
    throw ExpectedToken(TType::Ident, ts.get());
  }

  std::string name = ts.get().lexeme;

  if (NLITERAL(ts.next(), "=")) {
    throw ExpectedToken("=", ts.get());
  }
  ts.next();

  auto value = parse_expr();

  return to_nodep(VarDeclNode(name, value, modfs, is_const, is_static));
}

NodeP Parser::parse_funcdecl(std::vector<lexer::TokenType> modfs) {
  ST_RULE("parse_funcdecl");

  if (NMATCH(ts.next(), TType::Ident)) {
    throw ExpectedToken(TType::Ident, ts.get());
  }

  std::string name = ts.get().lexeme;
  ts.next();

  auto params = parse_parameters();

  NodeP ret_type = to_nodep(TypeNode("void"));
  if (LITERAL(ts.get(), ":")) {
    ts.next();
    ret_type = parse_type();
  }

  NodeP block = parse_block();

  return to_nodep(FuncDeclNode(name, modfs, block, params, ret_type));
}

std::vector<std::pair<std::string, NodeP>> Parser::parse_parameters() {
  ST_RULE("parse_parameters");

  std::vector<std::pair<std::string, NodeP>> params;

  if (NLITERAL(ts.get(), "(")) {
    throw ExpectedToken("(", ts.get());
  }
  ts.next();

  if (LITERAL(ts.get(), ")")) {
    ts.next();
    return params;
  }

  while (true) {
    if (NMATCH(ts.get(), TType::Ident)) {
      throw ExpectedToken(TType::Ident, ts.get());
    }
    std::string name = ts.get().lexeme;
    ts.next();

    if (NLITERAL(ts.get(), ":")) {
      throw ExpectedToken(":", ts.get());
    }
    ts.next();

    NodeP type = parse_type();
    params.emplace_back(name, type);

    if (LITERAL(ts.get(), ")")) {
      ts.next();
      break;
    }

    if (NLITERAL(ts.get(), ",")) {
      throw ExpectedToken(",", ts.get());
    }
    ts.next();
  }

  return params;
}

NodeP Parser::parse_classdecl() {
  ST_RULE("parse_classdecl");
  TODO("Class declarations");
}

NodeP Parser::parse_structdecl() {
  ST_RULE("parse_structdecl");
  TODO("Structure declarations");
}

/* Expressions */

NodeP Parser::parse_type() {
  ST_RULE("parse_type");

  bool is_const = false;
  bool is_comptime = false;
  bool is_reference = false;
  bool is_throwable = false;
  bool is_nullable = false;

  while (true) {
    if (MATCH(ts.get(), TType::Const)) {
      is_const = true;
      ts.next();
    } else if (MATCH(ts.get(), TType::Comptime)) {
      is_comptime = true;
      ts.next();
    } else if (LITERAL(ts.get(), "&")) {
      is_reference = true;
      ts.next();
    } else if (LITERAL(ts.get(), "!")) {
      is_throwable = true;
      ts.next();
    } else {
      break;
    }
  }

  std::shared_ptr<TypeNode> node;

  if (LITERAL(ts.get(), "[")) {
    ts.next();

    int size = -1;
    if (MATCH(ts.get(), TType::Int)) {
      size = std::stoi(ts.get().lexeme);
      ts.next();
    }

    if (NLITERAL(ts.get(), "]")) {
      throw ExpectedToken("]", ts.get());
    }
    ts.next();

    auto elem_type = std::dynamic_pointer_cast<TypeNode>(parse_type());
    if (!elem_type) {
      throw std::runtime_error("Expected type after array size");
    }

    node = std::make_shared<TypeNode>(size, elem_type);

  } else if (LITERAL(ts.get(), "{")) {
    ts.next();

    auto key_type = std::dynamic_pointer_cast<TypeNode>(parse_type());
    if (!key_type) {
      throw std::runtime_error("Expected key type in map");
    }

    if (NLITERAL(ts.get(), "}")) {
      throw ExpectedToken("}", ts.get());
    }
    ts.next();

    auto value_type = std::dynamic_pointer_cast<TypeNode>(parse_type());
    if (!value_type) {
      throw std::runtime_error("Expected value type in map");
    }

    node = std::make_shared<TypeNode>(key_type, value_type);

  } else if (MATCH(ts.get(), TType::Ident)) {
    node = std::make_shared<TypeNode>(ts.get().lexeme);
    ts.next();
  } else {
    throw UnexpectedToken(ts.get(), "at start of type");
  }

  if (LITERAL(ts.get(), "?")) {
    is_nullable = true;
    ts.next();
  }

  node->is_const = is_const;
  node->is_comptime = is_comptime;
  node->is_reference = is_reference;
  node->is_throwable = is_throwable;
  node->is_nullable = is_nullable;

  return node;
}

NodeP Parser::parse_expr() {
  ST_RULE("parse_expr");

  if (LITERAL(ts.get(), "{")) {
    return parse_block();
  } else if (MATCH(ts.get(), TType::New)) {
    return parse_new();
  } else if (MATCH(ts.get(), TType::Throw)) {
    return parse_throw();
  }

  return pratt(PREC_LOWEST);
}

NodeP Parser::parse_throw() {
  if (NMATCH(ts.get(), TType::Throw)) {
    throw ExpectedToken(TType::Throw, ts.get());
  }
  ts.next();

  auto operand = parse_expr();

  return to_nodep(ThrowNode(operand));
}

NodeP Parser::parse_new() {
  ST_RULE("parse_new");

  if (NMATCH(ts.get(), TType::New)) {
    throw ExpectedToken(TType::New, ts.get());
  }
  ts.next();

  auto type = parse_type();

  if (NLITERAL(ts.get(), "(")) {
    return to_nodep(NewExprNode(type, {}));
  }

  ts.next();
  std::vector<NodeP> args;

  if (NLITERAL(ts.get(), ")")) {
    while (true) {
      args.push_back(pratt(PREC_ASSIGNMENT));

      if (LITERAL(ts.get(), ")")) {
        break;
      }

      if (NLITERAL(ts.get(), ",")) {
        throw ExpectedToken(",", ts.get());
      }
      ts.next();
    }
  }

  ts.next();
  return to_nodep(NewExprNode(type, args));
}

const std::set<std::string> unary_ops = {"-",      "*",     "discard", "typeof",
                                         "delete", "defer", "not",     "throw"};

static Precedence get_precedence(const lexer::Token &tk) {
  if (LITERAL(tk, "."))
    return PREC_CALL;
  if (LITERAL(tk, "("))
    return PREC_CALL;

  if (tk.kind == TType::Op) {
    if (LITERAL(tk, "="))
      return PREC_ASSIGNMENT;
    if (LITERAL(tk, "+") || LITERAL(tk, "-"))
      return PREC_TERM;
    if (LITERAL(tk, "*") || LITERAL(tk, "/"))
      return PREC_FACTOR;
  }

  return PREC_LOWEST;
}

NodeP Parser::parse_prefix() {
  ST_RULE("parse_prefix");

  auto tk = ts.get();

  if (MATCH(tk, TType::Int) || MATCH(tk, TType::Float) ||
      MATCH(tk, TType::String) || MATCH(tk, TType::Ident) ||
      MATCH(tk, TType::Bool) || MATCH(tk, TType::Null)) {
    ts.next();
    return to_nodep(LiteralNode(tk.lexeme, tk.kind));
  }

  if (LITERAL(tk, "(")) {
    ts.next();
    NodeP expr = pratt(PREC_LOWEST);

    if (NLITERAL(ts.get(), ")")) {
      throw ExpectedToken(")", ts.get());
    }

    ts.next();
    return expr;
  }

  if (MATCH(tk, TType::Op) && SET_HAS(unary_ops, tk.lexeme)) {
    std::string op = tk.lexeme;
    ts.next();
    NodeP right = pratt(PREC_PREFIX);
    return to_nodep(UnaryExprNode(op, right));
  }

  throw UnexpectedToken(tk, "at start of expression");
}

NodeP Parser::parse_infix(NodeP left, Precedence precedence) {
  ST_RULE("parse_infix");

  lexer::Token op_token = ts.get();

  if (LITERAL(op_token, "(")) {
    ts.next();
    std::vector<NodeP> args;

    if (NLITERAL(ts.get(), ")")) {
      while (true) {
        args.push_back(pratt(PREC_ASSIGNMENT));

        if (LITERAL(ts.get(), ")")) {
          break;
        }

        if (NLITERAL(ts.get(), ",")) {
          throw ExpectedToken(",", ts.get());
        }
        ts.next();
      }
    }

    ts.next();
    return to_nodep(CallExprNode(left, args));
  }

  if (LITERAL(op_token, ".")) {
    ts.next();

    if (NMATCH(ts.get(), TType::Ident)) {
      throw ExpectedToken(TType::Ident, ts.get());
    }

    std::string member = ts.get().lexeme;
    ts.next();

    return to_nodep(MemberAccessNode(left, member));
  }

  if (MATCH(op_token, TType::Unless)) {
    ts.next();
    auto right = parse_expr();
    if (MATCH(ts.get(), TType::Ifso)) {
      ts.next();
      auto ifso = parse_expr();
      return to_nodep(UnlessExprNode(left, right, ifso));
    } else {
      return to_nodep(UnlessExprNode(left, right));
    }
  }

  std::string op = op_token.lexeme;
  Precedence op_prec = get_precedence(op_token);
  ts.next();

  NodeP right = pratt(op_prec);
  return to_nodep(BinaryExprNode(left, op, right));
}

NodeP Parser::pratt(int precedence) {
  ST_RULE("pratt");

  NodeP left = parse_prefix();

  while (true) {
    lexer::Token tk = ts.get();
    Precedence next_prec = get_precedence(tk);

    if (next_prec <= precedence)
      break;

    left = parse_infix(left, next_prec);
  }

  return left;
}

NodeP Parser::parse_include() {
  ST_RULE("parse_include");

  std::string from;
  std::vector<std::string> includes = {};

  if (MATCH(ts.get(), TType::From)) {
    if (NMATCH(ts.next(), TType::String)) {
      throw ExpectedToken(TType::String, ts.get());
    }

    from = ts.get().lexeme;

    if (NMATCH(ts.next(), TType::Include)) {
      throw ExpectedToken("include", ts.get());
    }
    ts.next();

    while (true) {
      if (!ts.has_next()) {
        throw UnexpectedEOI();
      }

      auto tk = ts.get();

      if (MATCH(tk, TType::EOS) || MATCH(tk, TType::EOI)) {
        ts.next();
        break;
      }

      if (LITERAL(tk, ",")) {
        tk = ts.next();
      } else if (MATCH(tk, TType::Ident)) {
        includes.push_back(tk.lexeme);
        ts.next();
      } else {
        throw ExpectedToken(TType::Ident, ts.get());
      }
    }
  } else if (MATCH(ts.get(), TType::Include)) {
    if (NMATCH(ts.next(), TType::String)) {
      throw ExpectedToken(TType::String, ts.get());
    }

    from = ts.get().lexeme;
  } else {
    throw ExpectedOneOfTokens({TType::Include, TType::From}, ts.get());
  }

  return to_nodep(IncludeNode(from, includes));
}

NodeP Parser::parse_block() {
  ST_RULE("parse_block");

  if (NLITERAL(ts.get(), "{")) {
    throw ExpectedToken("{", ts.get());
  }
  ts.next();

  std::vector<NodeP> nodes;
  while (NLITERAL(ts.get(), "}")) {
    if (MATCH(ts.get(), TType::EOI) || LITERAL(ts.peek(1), "}")) {
      break;
    }
    nodes.push_back(parse_start());
    if (MATCH(ts.get(), TType::EOS)) {
      ts.next();
    }
  }
  ts.next();
  return to_nodep(BlockNode(nodes));
}

} // namespace parser
} // namespace vanadium
