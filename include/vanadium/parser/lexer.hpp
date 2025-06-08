#ifndef INCLUDE_PARSER_LEXER_HPP_
#define INCLUDE_PARSER_LEXER_HPP_

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace vanadium {
namespace lexer {

enum class TokenType {
  // End of input
  EOI,

  // Primitive
  Ident,
  Float,
  Int,
  String,
  Op,
  Punct,
  Bool,
  Null,

  // keywords
  If,
  Else,
  Elif,
  While,
  For,
  In,
  Repeat,
  Until,
  Defer,
  Delete,
  Match,
  Case,
  Default,
  Func,
  Return,
  Class,
  Public,
  Private,
  Override,
  Struct,
  Iface,
  Impl,
  Enum,
  Let,
  Const,
  Static,
  Export,
  Discard,
  From,
  Include,
  Typeof,
  Throw,
  Try,
  Catch,
  Guard,
  As,
  Unless,
  Ifso,
  Ifnot,
  New,
  Destruct,
  And,
  Or,
  Not,
  Sealed,
  Comptime,

  // Misc.
  EOS,
};

extern std::string type_as_string(TokenType type);

const std::map<std::string, TokenType> keyword_map = {
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"elif", TokenType::Elif},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"in", TokenType::In},
    {"repeat", TokenType::Repeat},
    {"until", TokenType::Until},
    {"defer", TokenType::Defer},
    {"delete", TokenType::Delete},
    {"match", TokenType::Match},
    {"case", TokenType::Case},
    {"default", TokenType::Default},
    {"func", TokenType::Func},
    {"return", TokenType::Return},
    {"class", TokenType::Class},
    {"public", TokenType::Public},
    {"private", TokenType::Private},
    {"override", TokenType::Override},
    {"struct", TokenType::Struct},
    {"iface", TokenType::Iface},
    {"impl", TokenType::Impl},
    {"enum", TokenType::Enum},
    {"let", TokenType::Let},
    {"const", TokenType::Const},
    {"static", TokenType::Static},
    {"export", TokenType::Export},
    {"discard", TokenType::Discard},
    {"from", TokenType::From},
    {"include", TokenType::Include},
    {"typeof", TokenType::Typeof},
    {"throw", TokenType::Throw},
    {"try", TokenType::Try},
    {"catch", TokenType::Catch},
    {"guard", TokenType::Guard},
    {"as", TokenType::As},
    {"unless", TokenType::Unless},
    {"ifso", TokenType::Ifso},
    {"ifnot", TokenType::Ifnot},
    {"new", TokenType::New},
    {"and", TokenType::And},
    {"or", TokenType::Or},
    {"not", TokenType::Not},
    {"sealed", TokenType::Sealed},
    {"comptime", TokenType::Comptime},
    {"true", TokenType::Bool},
    {"false", TokenType::Bool},
    {"null", TokenType::Null}};

const std::set<char> op_list = {'-', '+', '*', '/', '^', '=',
                                '<', '>', '!', '?', '&', '|'};
const std::set<char> punct_list = {'(', ')', '{', '}', '[', ']', ':', '.', ','};

struct TokenPos {
  long from;
  long to;
  size_t line;
  TokenPos(int from, int to) : from(from), to(to), line(-1) {};
  TokenPos(int from, int to, size_t line) : from(from), to(to), line(line) {};

  std::string as_string() const {
    return std::string((line != -1) ? std::to_string(line) : "unknown") + ":" +
           ((from != -1) ? std::to_string(from) : "unknown") + "-" +
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
  Token(TokenType type, std::string lexeme, int from, int to, size_t line)
      : kind(type), lexeme(lexeme), pos(from, to, line) {}

  void display() const {
    std::cout << "Type: " << type_as_string(kind) << ", Lexeme: '" << lexeme
              << "', Pos: " << pos.as_string() << std::endl;
  };
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
  Token get();
  bool has_next();
  bool next_is_eoi();
  Token peek(size_t offset);
  const size_t get_index();
  const TokenList get_tokens();

private:
  TokenList tokens;
  size_t current;
};

extern TokenStream tokenize(std::string input);

} // namespace lexer
} // namespace vanadium

#endif // INCLUDE_PARSER_LEXER_HPP_
