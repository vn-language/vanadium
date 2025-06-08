#ifndef INCLUDE_PARSER_AST_HPP_
#define INCLUDE_PARSER_AST_HPP_

#include "vanadium/diagnostics/colors.hpp"
#include "vanadium/parser/lexer.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vanadium {
namespace parser {

enum class NodeKind {
  Include,
  Block,
  FuncDecl,
  VarDecl,
  BinaryExpr,
  UnaryExpr,
  Literal,
  CallExpr,
  MemberAccess,
  EOI,
  Type,
  ImplicitReturn,
  NewExpr,
  Throw,
  UnlessExpr
};

const std::map<NodeKind, std::string> node_kind_map = {
    {NodeKind::Include, "Include"},
    {NodeKind::Block, "Block"},
    {NodeKind::FuncDecl, "FuncDecl"},
    {NodeKind::VarDecl, "VarDecl"},
    {NodeKind::BinaryExpr, "BinaryExpr"},
    {NodeKind::UnaryExpr, "UnaryExpr"},
    {NodeKind::Literal, "Literal"},
    {NodeKind::CallExpr, "CallExpr"},
    {NodeKind::MemberAccess, "MemberAccess"},
    {NodeKind::EOI, "End of input"},
    {NodeKind::Type, "Type"},
    {NodeKind::ImplicitReturn, "ImplicitReturn"},
    {NodeKind::NewExpr, "NewExpr"},
    {NodeKind::Throw, "Throw"}};

extern std::string node_kind_as_string(NodeKind kind);

class Node {
public:
  NodeKind kind;
  virtual ~Node() = default;

  virtual const std::string as_string() {
    return "Node(\n"
           "    Kind: '" +
           node_kind_as_string(kind) +
           "'\n"
           ")";
  }
};

typedef std::shared_ptr<Node> NodeP;

class IncludeNode : public Node {
public:
  IncludeNode(std::string from, std::vector<std::string> includes)
      : from(from), includes(includes) {}

  const NodeKind kind = NodeKind::Include;
  std::string from;
  std::vector<std::string> includes;

  const std::string as_string() override {
    std::string joined_includes;
    for (size_t i = 0; i < includes.size(); ++i) {
      joined_includes += "'" + includes[i] + "'";
      if (i + 1 < includes.size())
        joined_includes += ",\n        ";
    }

    return "Node(Kind: '" + node_kind_as_string(kind) + "', From: '" + from +
           "', Includes: [" + joined_includes + "])";
  }
};

class BlockNode : public Node {
public:
  BlockNode(std::vector<NodeP> inner) : inner(inner) {}

  const NodeKind kind = NodeKind::Block;
  std::vector<NodeP> inner;

  const std::string as_string() override {
    std::string joined_inner;
    for (const auto &node : inner)
      joined_inner += "  " + node->as_string() + ",\n";

    return "Node(Kind: '" + node_kind_as_string(kind) + "', Inner: [\n" +
           joined_inner + "])";
  }
};

class FuncDeclNode : public Node {
public:
  FuncDeclNode(std::string name, std::vector<lexer::TokenType> modifiers,
               NodeP block, std::vector<std::pair<std::string, NodeP>> params,
               NodeP ret_type)
      : name(name), modfs(modifiers), block(block), parameters(params),
        ret_type(ret_type) {}

  const NodeKind kind = NodeKind::FuncDecl;
  NodeP block;
  std::vector<lexer::TokenType> modfs;
  std::vector<std::pair<std::string, NodeP>> parameters;
  std::string name;
  NodeP ret_type;

  const std::string as_string() override {
    std::string joined_modfs = "[";
    for (size_t i = 0; i < modfs.size(); ++i) {
      joined_modfs += "'" + lexer::type_as_string(modfs[i]) + "'";
      if (i + 1 < modfs.size())
        joined_modfs += ",\n        ";
    }
    joined_modfs += "]";

    std::string joined_params;
    if (parameters.size() > 0) {
      for (size_t i = 0; i < parameters.size(); ++i) {
        joined_params +=
            parameters[i].first + ": " + parameters[i].second->as_string();
        if (i + 1 < parameters.size())
          joined_params += ", ";
      }
    } else {
      joined_params = "None";
    }

    return "Node(Kind: '" + node_kind_as_string(kind) + "', Name: '" + name +
           "', Modifiers: " + joined_modfs + ", Block: " + block->as_string() +
           ", Params: " + joined_params +
           ", Returns: " + ret_type->as_string() + ")";
  }
};

class VarDeclNode : public Node {
public:
  VarDeclNode(std::string name, NodeP value,
              std::vector<lexer::TokenType> modifiers, bool is_const,
              bool is_static)
      : name(name), value(value), modfs(modifiers), is_const(is_const),
        is_static(is_static) {}

  const NodeKind kind = NodeKind::VarDecl;
  NodeP value;
  std::vector<lexer::TokenType> modfs;
  std::string name;
  bool is_const;
  bool is_static;

  const std::string as_string() override {
    std::string joined_modfs = "[";
    for (size_t i = 0; i < modfs.size(); ++i) {
      joined_modfs += "'" + lexer::type_as_string(modfs[i]) + "'";
      if (i + 1 < modfs.size())
        joined_modfs += ",\n        ";
    }
    joined_modfs += "]";

    return "Node(Kind: '" + node_kind_as_string(kind) + "', Name: '" + name +
           "', Value: " + value->as_string() + ", Modifiers: " + joined_modfs +
           ", Constant: " + (is_const ? "yes" : "no") +
           ", Static: " + (is_static ? "yes" : "no") + ")";
  }
};

class BinaryExprNode : public Node {
public:
  BinaryExprNode(NodeP lhs, std::string op, NodeP rhs)
      : lhs(lhs), op(op), rhs(rhs) {}

  const NodeKind kind = NodeKind::BinaryExpr;
  NodeP lhs;
  NodeP rhs;
  std::string op;

  const std::string as_string() override {
    return "Node(Kind: '" + node_kind_as_string(kind) +
           "', Left: " + lhs->as_string() + ", Right: " + rhs->as_string() +
           ", Operator: '" + op + "')";
  }
};

class UnaryExprNode : public Node {
public:
  UnaryExprNode(std::string op, NodeP operand) : op(op), operand(operand) {}

  const NodeKind kind = NodeKind::UnaryExpr;
  std::string op;
  NodeP operand;

  const std::string as_string() override {
    return "Node(Kind: '" + node_kind_as_string(kind) + "', Operator: '" + op +
           "', Operand: " + operand->as_string() + ")";
  }
};

class LiteralNode : public Node {
public:
  LiteralNode(std::string value, lexer::TokenType type)
      : value(value), type(type) {}

  const NodeKind kind = NodeKind::Literal;
  std::string value;
  lexer::TokenType type;

  const std::string as_string() override {
    return "Node(Kind: '" + node_kind_as_string(kind) + "', Type: '" +
           lexer::type_as_string(type) + "', Value: '" + value + "')";
  }
};

class CallExprNode : public Node {
public:
  CallExprNode(NodeP callee, std::vector<NodeP> args)
      : callee(callee), args(args) {}

  const NodeKind kind = NodeKind::CallExpr;
  NodeP callee;
  std::vector<NodeP> args;

  const std::string as_string() override {
    std::string joined_args;
    for (const auto &arg : args)
      joined_args += "  " + arg->as_string() + ",\n";

    return "Node(Kind: '" + node_kind_as_string(kind) +
           "', Callee: " + callee->as_string() + ", Args: [\n" + joined_args +
           "])";
  }
};

class MemberAccessNode : public Node {
public:
  MemberAccessNode(NodeP object, std::string member)
      : object(object), member(member) {}

  const NodeKind kind = NodeKind::MemberAccess;
  NodeP object;
  std::string member;

  const std::string as_string() override {
    return "Node(Kind: '" + node_kind_as_string(kind) +
           "', Object: " + object->as_string() + ", Member: '" + member + "')";
  }
};

class EOINode : public Node {
public:
  EOINode() = default;
  const NodeKind kind = NodeKind::EOI;

  const std::string as_string() override {
    return "Node(Kind: '" + node_kind_as_string(kind) + "')";
  }
};

struct TypeNode : public Node {
  const NodeKind kind = NodeKind::Type;

  enum class TypeKind { Simple, Array, Map };
  const TypeKind type_kind;

  bool is_const = false;
  bool is_comptime = false;
  bool is_reference = false;
  bool is_throwable = false;
  bool is_nullable = false;

  std::string base_name;
  int array_size = -1;
  std::shared_ptr<TypeNode> element_type;
  std::shared_ptr<TypeNode> key_type;
  std::shared_ptr<TypeNode> value_type;

  TypeNode(std::string base_name_)
      : type_kind(TypeKind::Simple), base_name(std::move(base_name_)) {}

  TypeNode(int size, std::shared_ptr<TypeNode> elem_type)
      : type_kind(TypeKind::Array), array_size(size),
        element_type(std::move(elem_type)) {}

  TypeNode(std::shared_ptr<TypeNode> key, std::shared_ptr<TypeNode> value)
      : type_kind(TypeKind::Map), key_type(std::move(key)),
        value_type(std::move(value)) {}

  const std::string as_string() override {
    std::string prefix;
    if (is_const)
      prefix += "const ";
    if (is_comptime)
      prefix += "comptime ";
    if (is_reference)
      prefix += "&";
    if (is_throwable)
      prefix += "!";

    std::string core;
    switch (type_kind) {
    case TypeKind::Simple:
      core = base_name;
      break;
    case TypeKind::Array:
      core = "[";
      if (array_size >= 0)
        core += std::to_string(array_size);
      core += "]";
      core += element_type->as_string();
      break;
    case TypeKind::Map:
      core = "{" + key_type->as_string() + "}" + value_type->as_string();
      break;
    }

    std::string suffix = is_nullable ? "?" : "";
    return "Node(Kind: '" + node_kind_as_string(kind) + "', Value: '" + prefix +
           core + suffix + "')";
  }
};

class ImplicitReturnNode : public Node {
public:
  ImplicitReturnNode(NodeP value) : value(value) {};

  const NodeKind kind = NodeKind::ImplicitReturn;
  NodeP value;

  const std::string as_string() override {
    return "Node(Kind: '" + node_kind_as_string(kind) +
           "', Value: " + value->as_string() + ")";
  }
};

class NewExprNode : public Node {
public:
  NewExprNode(NodeP type, std::vector<NodeP> args) : type(type), args(args) {}

  const NodeKind kind = NodeKind::NewExpr;
  NodeP type;
  std::vector<NodeP> args;

  const std::string as_string() override {
    std::string joined_args;
    for (const auto &arg : args)
      joined_args += "  " + arg->as_string() + ",\n";

    return "Node(Kind '" + node_kind_as_string(kind) +
           "', Type: " + type->as_string() + ", Args: [" + joined_args + "])";
  }
};

class ThrowNode : public Node {
public:
  ThrowNode(NodeP operand) : operand(operand) {};

  const NodeKind kind = NodeKind::Throw;
  NodeP operand;

  const std::string as_string() override {
    return "Node(Kind: '" + node_kind_as_string(kind) +
           "', Throwed: " + operand->as_string() + ")";
  }
};

class UnlessExprNode : public Node {
public:
  UnlessExprNode(NodeP lhs, NodeP rhs) : lhs(lhs), rhs(rhs), has_ifso(false) {}

  UnlessExprNode(NodeP lhs, NodeP rhs, NodeP ifso)
      : lhs(lhs), rhs(rhs), has_ifso(true), ifso(ifso) {}

  const NodeKind kind = NodeKind::UnlessExpr;
  NodeP lhs;
  NodeP rhs;

  bool has_ifso;
  NodeP ifso;
};

template <typename NodeType> inline NodeP to_nodep(NodeType node) {
  return std::make_shared<NodeType>(node);
};

} // namespace parser
} // namespace vanadium

#endif // INCLUDE_PARSER_AST_HPP_
