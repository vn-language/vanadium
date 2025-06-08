#ifndef INCLUDE_DIAGNOSTICS_DIAGNOSTICS_HPP_
#define INCLUDE_DIAGNOSTICS_DIAGNOSTICS_HPP_

#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace vanadium {
namespace diagnostics {

enum class Severity { Error, Warning, Note };

struct Label {
  std::string title;
  std::string message;
  int line;
  int col;

  Label(const std::string &msg, const std::string &title, int line, int col)
      : message(msg), title(title), line(line), col(col) {
    has_pos = true;
  }
  Label(const std::string &msg, const std::string &title)
      : message(msg), title(title) {
    has_pos = false;
  }

  const bool has_pos_info() const { return has_pos; }

private:
  bool has_pos;
};

struct Diagnostic {
  Diagnostic(Severity severity, const std::string &title,
             const std::string &msg)
      : severity(severity), title(title), message(msg) {};

  void print(std::ostream &os = std::cout) const;
  void add_label(Label label);

private:
  Severity severity;
  std::string message;
  std::string title;
  std::vector<Label> labels;
};

} // namespace diagnostics
} // namespace vanadium

#endif // INCLUDE_DIAGNOSTICS_DIAGNOSTICS_HPP_
