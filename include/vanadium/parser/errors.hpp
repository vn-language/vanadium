#ifndef INCLUDE_PARSER_ERRORS_HPP_
#define INCLUDE_PARSER_ERRORS_HPP_

#include "vanadium/diagnostics/colors.hpp"
#include "vanadium/parser/lexer.hpp"
#include <exception>
#include <sstream>
#include <string>

namespace vanadium {
namespace parser {

class ParseError : public std::exception {
private:
  std::string message;

public:
  ParseError(const std::string &msg) : message(msg) {}

  const char *what() const throw() { return message.c_str(); }
};

class UnexpectedChar : public ParseError {
public:
  UnexpectedChar(char ch, int index, const std::string &notes = "")
      : ParseError("Unexpected char: '" GRN + std::string(1, ch) +
                   CRESET "' at " MAG + std::to_string(index) + CRESET +
                   (!notes.empty() ? (": " + notes) : "")) {}
};

class UnexpectedToken : public ParseError {
public:
  UnexpectedToken(lexer::Token token, const std::string &notes = "")
      : ParseError("Unexpected token: '" GRN + token.lexeme +
                   CRESET "' at " MAG + token.pos.as_string() + CRESET +
                   (!notes.empty() ? (": " + notes) : "")) {}
};

class ExpectedToken : public ParseError {
public:
  ExpectedToken(const std::string &expected, lexer::Token got,
                const std::string &notes = "")
      : ParseError("Expected token '" GRN + expected +
                   CRESET "' but got '" GRN + got.lexeme + CRESET "'" +
                   (!notes.empty() ? (": " + notes) : "")) {}

  ExpectedToken(lexer::TokenType expected, lexer::Token got,
                const std::string &notes = "")
      : ParseError("Expected token " BLU + lexer::type_as_string(expected) +
                   CRESET " but got '" GRN + got.lexeme + CRESET "'" +
                   (!notes.empty() ? (": " + notes) : "")) {}
};

class ExpectedOneOfTokens : public ParseError {
public:
  ExpectedOneOfTokens(const std::vector<std::string> &expected,
                      lexer::Token got, const std::string &notes = "")
      : ParseError(construct_message(expected, got, notes)) {}

  ExpectedOneOfTokens(const std::vector<lexer::TokenType> &expected,
                      lexer::Token got, const std::string &notes = "")
      : ParseError(construct_message(expected, got, notes)) {}

private:
  std::string construct_message(const std::vector<std::string> &expected,
                                lexer::Token got, const std::string &notes) {
    std::stringstream msg;

    if (expected.size() == 1) {
      msg << "Expected token: '" << expected[0] << "' ";
    } else {
      msg << "Expected either ";
      for (size_t i = 0; i < expected.size(); ++i) {
        if (i > 0)
          msg << ", ";
        msg << "'" << expected[i] << "'";
      }
      msg << " ";
    }

    msg << "but got '" << got.lexeme << "'";

    if (!notes.empty()) {
      msg << "[: " << notes << "]";
    }

    return msg.str();
  }

  std::string
  construct_message(const std::vector<lexer::TokenType> &expected_types,
                    lexer::Token got, const std::string &notes) {
    std::stringstream msg;
    std::vector<std::string> expected;
    for (auto type : expected_types) {
      expected.push_back(lexer::type_as_string(type));
    }
    return construct_message(expected, got, notes);
  }
};

class UnexpectedEOI : public ParseError {
public:
  UnexpectedEOI(const std::string &notes = "")
      : ParseError("Unexpected end of input" +
                   (!notes.empty() ? (": " + notes) : "")) {}
};

class SyntaxError : public ParseError {
public:
  SyntaxError(const std::string &details)
      : ParseError("Syntax error: " + details) {}
};

class InvalidToken : public ParseError {
public:
  InvalidToken(const lexer::Token &token, const std::string &reason = "")
      : ParseError("Invalid token '" + token.lexeme + "' at " +
                   token.pos.as_string() +
                   (!reason.empty() ? (": " + reason) : "")) {}
};

} // namespace parser
} // namespace vanadium

#endif // INCLUDE_PARSER_ERRORS_HPP_
