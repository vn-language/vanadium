#include "vanadium/diagnostics/diagnostics.hpp"
#include "vanadium/diagnostics/colors.hpp"
#include <iostream>
#include <ostream>
#include <string>

namespace vanadium {
namespace diagnostics {

void Diagnostic::print(std::ostream &os) const {
  const char *color_start = "";
  const char *color_end = CRESET;

  switch (severity) {
  case Severity::Error:
    color_start = RED;
    os << color_start << "error: ";
    break;
  case Severity::Warning:
    color_start = YEL;
    os << color_start << "warning: ";
    break;
  case Severity::Note:
    color_start = CYN;
    os << color_start << "note: ";
    break;
  }

  os << CRESET << CYN << title << CRESET "\n"
     << "| Message: " << message << color_end << "\n";

  if (!labels.empty())
    os << "\n" MAG << "Notes:" << CRESET "\n";
  for (const auto &label : labels) {
    os << BLU << label.title << CRESET "\n";
    if (label.has_pos_info()) {
      os << "| At: " MAG << label.line << ":" << label.col << CRESET "\n";
    }
    os << "| Notes: " << label.message << "\n";
  }

  os << std::endl;
}

void Diagnostic::add_label(Label label) { labels.push_back(label); }

} // namespace diagnostics
} // namespace vanadium
