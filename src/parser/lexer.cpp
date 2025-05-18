#include "vanadium/parser/lexer.hpp"
#include "vanadium/types.hpp"

#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

Token TokenStream::next() { return tokens.at(current++); }

bool TokenStream::has_next() { return tokens.size() >= current + 1; }
bool TokenStream::next_is_eoi() {
  return has_next() && next().kind != TokenType::EOI;
};

Token TokenStream::peek(ulong offset = 1) {
  return tokens.at(current + offset);
}

const ulong TokenStream::get_index() { return current; }

const TokenList TokenStream::get_tokens() { return tokens; }

#define PUSH_TOKEN(TOKEN)                                                      \
  tokens.push_back(TOKEN);                                                     \
  index++;                                                                     \
  continue;

#define NOT_OUT_OF_BOUNDS (input.size() >= index)
#define CHECK_NEXT_INDEX (input.size() >= (index + 1))
#define AT_INDEX input[index]

TokenStream tokenize(std::string input) {
  TokenList tokens;

  size_t index = 0;
  size_t line = 0;
  while (NOT_OUT_OF_BOUNDS) {
    const char &ch = AT_INDEX;
    if (ch == '\n' || ch == ' ') {
      if (ch == '\n') {
        line++;
      }
      index++;
      continue;
    }

    if (std::isdigit(ch)) {
      int from = index;
      std::string buf(1, ch);
      index++;
      while (CHECK_NEXT_INDEX && std::isdigit(AT_INDEX)) {
        buf += AT_INDEX;
        index++;
      }
      if (AT_INDEX == '.') {
        buf += ".";
        index++;
        while (CHECK_NEXT_INDEX && std::isdigit(AT_INDEX)) {
          buf += AT_INDEX;
          index++;
        }
      }
      int to = index;

      PUSH_TOKEN(Token(TokenType::Number, buf, from, to, line));
    }

    if (ch == '"') {
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

    if (std::isalnum(ch) || ch == '_') {
      int from = index;
      std::string buf(1, ch);
      index++;
      while (CHECK_NEXT_INDEX && (std::isalnum(AT_INDEX)) || AT_INDEX == '_') {
        buf += AT_INDEX;
        index++;
      }
      int to = index;

      PUSH_TOKEN(Token(TokenType::Ident, buf, from, to, line))
    }

    index++;
  }
  tokens.push_back(Token(TokenType::EOI, "", input.size(), input.size()));
  return TokenStream(tokens);
}
