#include "vanadium/parser/lexer.hpp"
#include "vanadium/types.hpp"
#include "vanadium/util_macros.hpp"

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
  size_t line = 1;

  while (NOT_OUT_OF_BOUNDS) {
    if (AT_INDEX == '\n' || AT_INDEX == ' ') {
      if (AT_INDEX == '\n') {
        line++;
      }
      index++;
      continue;
    }

    if (std::isdigit(AT_INDEX)) {
      int from = index;
      TokenType final_type;
      std::string buf(1, AT_INDEX);
      index++;
      while (CHECK_NEXT_INDEX && std::isdigit(AT_INDEX)) {
        buf += AT_INDEX;
        index++;
      }
      if (AT_INDEX == '.') {
        final_type = TokenType::Float;
        buf += ".";
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

    if (std::isalnum(AT_INDEX) || AT_INDEX == '_') {
      int from = index;
      std::string buf(1, AT_INDEX);
      index++;
      while (CHECK_NEXT_INDEX && (std::isalnum(AT_INDEX)) || AT_INDEX == '_') {
        buf += AT_INDEX;
        index++;
      }
      int to = index;

      if (keyword_map.count(buf) > 0) {
        PUSH_TOKEN(Token(keyword_map.at(buf), buf, from, to, line));
      } else {
        PUSH_TOKEN(Token(TokenType::Ident, buf, from, to, line))
      }
    }

    if (SET_HAS(op_list, AT_INDEX)) {
      PUSH_TOKEN(
          Token(TokenType::Op, std::string(1, AT_INDEX), index, index, line))
    } else if (SET_HAS(punct_list, AT_INDEX)) {
      PUSH_TOKEN(
          Token(TokenType::Punct, std::string(1, AT_INDEX), index, index, line))
    }

    index++;
  }
  tokens.push_back(Token(TokenType::EOI, "", input.size(), input.size()));
  return TokenStream(tokens);
}
