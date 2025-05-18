#ifndef INCLUDE_PARSER_LEXER_HPP_
#define INCLUDE_PARSER_LEXER_HPP_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "vanadium/types.hpp"

enum class TokenType { EOI, Number, String, Ident };
const std::map<int, std::string> token_type_str = {
    {0, "EOI"}, {1, "Number"}, {2, "String"}, {3, "Identifier"}};

struct TokenPos {
  long from;
  long to;
  ulong line;
  TokenPos(int from, int to) : from(from), to(to), line(-1) {};
  TokenPos(int from, int to, ulong line) : from(from), to(to), line() {};

  std::string as_string() const {
    return std::string((line != -1) ? std::to_string(line) : "unknown") + ":" +
           ((from != -1) ? std::to_string(from) : "unknown") + "," +
           ((to != -1) ? std::to_string(to) : "unknown");
  }
};

struct Token {
  TokenType kind;
  std::string lexeme;
  TokenPos pos;

  Token(TokenType type, std::string lexeme)
      : kind(type), lexeme(lexeme), pos(-1, -1) {}
  Token(TokenType type, std::string lexeme, TokenPos pos)
      : kind(type), lexeme(lexeme), pos(pos) {}
  Token(TokenType type, std::string lexeme, int from, int to)
      : kind(type), lexeme(lexeme), pos(from, to) {}
  Token(TokenType type, std::string lexeme, int from, int to, ulong line)
      : kind(type), lexeme(lexeme), pos(from, to, line) {}

  void display() const {
    std::cout << "Type: " << type_as_str() << ", Lexeme: '" << lexeme
              << "', Pos: " << pos.as_string() << std::endl;
  };

private:
  std::string type_as_str() const { return token_type_str.at((int)kind); }
};

typedef std::vector<Token> TokenList;

class TokenStream {
public:
  TokenStream(TokenList input_tokens) : tokens(input_tokens), current(0) {};
  TokenStream(TokenStream &&) = default;
  TokenStream(const TokenStream &) = default;
  TokenStream &operator=(TokenStream &&) = default;
  TokenStream &operator=(const TokenStream &) = default;
  ~TokenStream() = default;

  Token next();
  bool has_next();
  bool next_is_eoi();
  Token peek(ulong offset);
  const ulong get_index();
  const TokenList get_tokens();

private:
  TokenList tokens;
  ulong current;
};

extern TokenStream tokenize(std::string input);

#endif // INCLUDE_PARSER_LEXER_HPP_
