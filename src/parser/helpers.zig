const std = @import("std");
const lexer = @import("lexer.zig");
const Token = lexer.Token;
const TokenType = lexer.TokenType;

pub fn peek(tokens: []const Token, current: usize) ?Token {
    return if (current < tokens.len) tokens[current] else null;
}

pub fn match_token(tokens: []const Token, current: *usize, kind: TokenType) ?Token {
    const tok = peek(tokens, current.*) orelse return null;
    if (tok.kind == kind) {
        current.* += 1;
        return tok;
    }
    return null;
}

pub fn expect(tokens: []const Token, current: *usize, kind: TokenType) !Token {
    const tok = peek(tokens, current.*) orelse return error.UnexpectedEOF;
    if (tok.kind != kind) return error.UnexpectedToken;
    current.* += 1;
    return tok;
}

pub fn expect_literal(tokens: []const Token, current: *usize, literal: []const u8) !Token {
    const tok = peek(tokens, current.*) orelse return error.UnexpectedEOF;
    if (!std.mem.eql(u8, tok.value, literal)) return error.UnexpectedToken;
    current.* += 1;
    return tok;
}

pub fn match_keyword(tokens: []const Token, current: *usize, keyword: []const u8) ?Token {
    const tok = peek(tokens, current.*) orelse return null;
    if (tok.kind == .Keyword and std.mem.eql(u8, tok.value, keyword)) {
        current.* += 1;
        return tok;
    }
    return null;
}

pub fn match_operator(tokens: []const Token, current: *usize, op: []const u8) ?Token {
    const tok = peek(tokens, current.*) orelse return null;
    if (tok.kind == .Operator and std.mem.eql(u8, tok.value, op)) {
        current.* += 1;
        return tok;
    }
    return null;
}
