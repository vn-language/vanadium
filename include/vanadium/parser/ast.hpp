#ifndef INCLUDE_PARSER_AST_HPP_
#define INCLUDE_PARSER_AST_HPP_

#include <memory>
namespace vanadium {
namespace parser {

enum class NodeKind {

};

class Node {
public:
  NodeKind kind;
  virtual ~Node() = default;

private:
};

typedef std::shared_ptr<Node> NodeP;

} // namespace parser
} // namespace vanadium

#endif // INCLUDE_PARSER_AST_HPP_
