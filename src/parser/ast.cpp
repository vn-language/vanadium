#include "vanadium/parser/ast.hpp"
#include <string>
namespace vanadium {
namespace parser {

std::string node_kind_as_string(NodeKind kind) {
  if (node_kind_map.count(kind) > 0) {
    return node_kind_map.at(kind);
  }
  return "Unknown";
}

} // namespace parser
} // namespace vanadium
