#include "vanadium/parser/lexer.hpp"
#include "vanadium/parser/errors.hpp"
#include "vanadium/util_macros.hpp"

#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

namespace vanadium {
namespace lexer {

/*  TokenStream methods */
Token TokenStream::next() {
  current++;
  return tokens.at(current);
}
Token TokenStream::get() { return tokens.at(current); }

bool TokenStream::has_next() { return tokens.size() > current + 1; }
bool TokenStream::next_is_eoi() {
  return has_next() && peek(1).kind != TokenType::EOI;
};

Token TokenStream::peek(size_t offset = 1) {
  return tokens.at(current + offset);
}

const size_t TokenStream::get_index() { return current; }
const TokenList TokenStream::get_tokens() { return tokens; }

/* Main lexer logic */
#define PUSH_TOKEN(TOKEN)                                                      \
  tokens.push_back(TOKEN);                                                     \
  continue;

#define NOT_OUT_OF_BOUNDS (input.size() >= index)
#define CHECK_NEXT_INDEX (input.size() >= (index + 1))
#define AT_INDEX input[index]
#define AT_OFFSET(OFFSET) input[index + OFFSET]

TokenStream tokenize(std::string input) {
  TokenList tokens;

  size_t index = 0;
  size_t line = 1;

  while (CHECK_NEXT_INDEX) {
    if (AT_INDEX == '@' && CHECK_NEXT_INDEX && AT_OFFSET(1) == '@') {
      index += 2;
      while (CHECK_NEXT_INDEX && AT_INDEX != '\n') {
        index++;
      }
      continue;
    }

    if (AT_INDEX == '@' && CHECK_NEXT_INDEX && AT_OFFSET(1) == '*') {
      index += 2;
      while (index + 1 < input.size() &&
             !(AT_INDEX == '*' && AT_OFFSET(1) == '@')) {
        if (AT_INDEX == '\n') {
          line++;
        }
        index++;
      }
      if (index + 1 < input.size()) {
        index += 2;
      }
      continue;
    }

    if (AT_INDEX == '\n' || AT_INDEX == ' ' || AT_INDEX == '\t') {
      if (AT_INDEX == '\n') {
        line++;
      }
      index++;
      continue;
    }

    if (std::isdigit(static_cast<unsigned char>(AT_INDEX))) {
      int from = index;
      TokenType final_type;
      std::string buf(1, AT_INDEX);
      index++;

      while (CHECK_NEXT_INDEX && std::isdigit(AT_INDEX)) {
        buf += AT_INDEX;
        index++;
      }

      if (CHECK_NEXT_INDEX && AT_INDEX == '.') {
        final_type = TokenType::Float;
        buf += '.';
        index++;
        while (CHECK_NEXT_INDEX && std::isdigit(AT_INDEX)) {
          buf += AT_INDEX;
          index++;
        }
      } else {
        final_type = TokenType::Int;
      }

      int to = index;
      PUSH_TOKEN(Token(final_type, buf, from, to, line));
    }

    if (AT_INDEX == '"') {
      int from = index;
      std::string buf;
      index++;
      while (CHECK_NEXT_INDEX && AT_INDEX != '"') {
        buf += AT_INDEX;
        index++;
      }

      index++;
      int to = index;
      PUSH_TOKEN(Token(TokenType::String, buf, from, to, line));
    }

    if (std::isalnum(static_cast<unsigned char>(AT_INDEX)) || AT_INDEX == '_' ||
        AT_INDEX == '!' || AT_INDEX == '?') {
      int from = index;
      std::string buf(1, AT_INDEX);
      index++;

      while (CHECK_NEXT_INDEX &&
             (std::isalnum(static_cast<unsigned char>(AT_INDEX)) ||
              AT_INDEX == '_' || AT_INDEX == '!' || AT_INDEX == '?')) {
        buf += AT_INDEX;
        index++;
      }

      int to = index;
      if (keyword_map.count(buf) > 0) {
        PUSH_TOKEN(Token(keyword_map.at(buf), buf, from, to, line));
      } else {
        PUSH_TOKEN(Token(TokenType::Ident, buf, from, to, line));
      }
    }

    if (SET_HAS(op_list, AT_INDEX)) {
      int from = index;
      std::string lex(1, AT_INDEX);
      index++;
      int to = index;
      PUSH_TOKEN(Token(TokenType::Op, lex, from, to, line));
    }

    if (SET_HAS(punct_list, AT_INDEX)) {
      int from = index;
      std::string lex(1, AT_INDEX);
      index++;
      int to = index;
      PUSH_TOKEN(Token(TokenType::Punct, lex, from, to, line));
    }

    if (AT_INDEX == ';') {
      index++;
      PUSH_TOKEN(Token(TokenType::EOS, ";", index, index, line));
    }

    throw parser::UnexpectedChar(AT_INDEX, index);
  }

  tokens.push_back(Token(TokenType::EOI, "", input.size(), input.size()));
  return TokenStream(tokens);
}

/* misc. */
std::string type_as_string(TokenType type) {
  switch (type) {
  case TokenType::EOI:
    return "EOI";
  case TokenType::Float:
    return "Float";
  case TokenType::Int:
    return "Int";
  case TokenType::String:
    return "String";
  case TokenType::Ident:
    return "Identifier";
  case TokenType::Op:
    return "Operator";
  case TokenType::Punct:
    return "Punctuation";

  case TokenType::If:
    return "If";
  case TokenType::Else:
    return "Else";
  case TokenType::Elif:
    return "Elif";
  case TokenType::While:
    return "While";
  case TokenType::For:
    return "For";
  case TokenType::In:
    return "In";
  case TokenType::Repeat:
    return "Repeat";
  case TokenType::Until:
    return "Until";
  case TokenType::Defer:
    return "Defer";
  case TokenType::Delete:
    return "Delete";
  case TokenType::Match:
    return "Match";
  case TokenType::Case:
    return "Case";
  case TokenType::Default:
    return "Default";
  case TokenType::Func:
    return "Func";
  case TokenType::Return:
    return "Return";
  case TokenType::Class:
    return "Class";
  case TokenType::Public:
    return "Public";
  case TokenType::Private:
    return "Private";
  case TokenType::Override:
    return "Override";
  case TokenType::Struct:
    return "Struct";
  case TokenType::Iface:
    return "Iface";
  case TokenType::Impl:
    return "Impl";
  case TokenType::Enum:
    return "Enum";
  case TokenType::Let:
    return "Let";
  case TokenType::Const:
    return "Const";
  case TokenType::Static:
    return "Static";
  case TokenType::Discard:
    return "Discard";
  case TokenType::From:
    return "From";
  case TokenType::Include:
    return "Include";
  case TokenType::Typeof:
    return "Typeof";
  case TokenType::Throw:
    return "Throw";
  case TokenType::Try:
    return "Try";
  case TokenType::Catch:
    return "Catch";
  case TokenType::Guard:
    return "Guard";
  case TokenType::As:
    return "As";
  case TokenType::Unless:
    return "Unless";
  case TokenType::Ifso:
    return "Ifso";
  case TokenType::Ifnot:
    return "Ifnot";
  case TokenType::New:
    return "New";
  case TokenType::Destruct:
    return "Destruct";
  case TokenType::And:
    return "And";
  case TokenType::Or:
    return "Or";
  case TokenType::Not:
    return "Not";
  case TokenType::EOS:
    return "EOS";
  case TokenType::Bool:
    return "Boolean";
  case TokenType::Null:
    return "Null";

  default:
    throw std::invalid_argument("Unknown TokenType");
  }
}

} // namespace lexer
} // namespace vanadium
