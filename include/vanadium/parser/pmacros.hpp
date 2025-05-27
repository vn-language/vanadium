#ifndef INCLUDE_PARSER_PMACROS_HPP_
#define INCLUDE_PARSER_PMACROS_HPP_

#define MATCH(TK, TP) (TK.kind == TP)
#define NMATCH(TK, TP) (TK.kind != TP)
#define LITERAL(TK, L) (TK.lexeme == L)
#define NLITERAL(TK, L) (TK.lexeme != L)

#endif // INCLUDE_PARSER_PMACROS_HPP_
