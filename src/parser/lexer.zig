const std = @import("std");
const utils = @import("../utils.zig");

pub const TokenType = enum {
    Identifier,
    Keyword, //Type,
    Number,
    String,
    Whitespace,
    EOF,
    Operator,
    Punctuation,
    Unknown,
};

pub const Token = struct {
    kind: TokenType,
    value: []const u8,
    position: usize,
    line: usize,
    is_heap_allocated: bool,
};

fn is_alpha(c: u8) bool {
    return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or c == '_';
}

fn is_digit(c: u8) bool {
    return c >= '0' and c <= '9';
}

fn is_whitespace(c: u8) bool {
    return c == ' ' or c == '\t' or c == '\n' or c == '\r';
}

const operator_map = std.StaticStringMap(TokenType).initComptime(&.{ .{ "+", TokenType.Operator }, .{ "-", TokenType.Operator }, .{ "*", TokenType.Operator }, .{ "/", TokenType.Operator }, .{ "=", TokenType.Operator }, .{ "==", TokenType.Operator }, .{ "!=", TokenType.Operator }, .{ "<", TokenType.Operator }, .{ "<=", TokenType.Operator }, .{ ">", TokenType.Operator }, .{ ">=", TokenType.Operator }, .{ "&&", TokenType.Operator }, .{ "||", TokenType.Operator }, .{ "&", TokenType.Operator }, .{ "*", TokenType.Operator } });

const punctuation_map = std.StaticStringMap(TokenType).initComptime(&.{ .{ ";", TokenType.Punctuation }, .{ "(", TokenType.Punctuation }, .{ ")", TokenType.Punctuation }, .{ "[", TokenType.Punctuation }, .{ "]", TokenType.Punctuation }, .{ "{", TokenType.Punctuation }, .{ "}", TokenType.Punctuation }, .{ ",", TokenType.Punctuation }, .{ ":", TokenType.Punctuation }, .{ ".", TokenType.Punctuation } });

const keyword_map = std.StaticStringMap(TokenType).initComptime(&.{ .{ "if", TokenType.Keyword }, .{ "else", TokenType.Keyword }, .{ "elif", TokenType.Keyword }, .{ "while", TokenType.Keyword }, .{ "for", TokenType.Keyword }, .{ "until", TokenType.Keyword }, .{ "include", TokenType.Keyword }, .{ "defer", TokenType.Keyword }, .{ "switch", TokenType.Keyword }, .{ "case", TokenType.Keyword }, .{ "default", TokenType.Keyword }, .{ "fn", TokenType.Keyword }, .{ "return", TokenType.Keyword }, .{ "struct", TokenType.Keyword }, .{ "new", TokenType.Keyword }, .{ "macro", TokenType.Keyword }, .{ "interface", TokenType.Keyword }, .{ "using", TokenType.Keyword }, .{ "enum", TokenType.Keyword }, .{ "mut", TokenType.Keyword }, .{ "imut", TokenType.Keyword } });

// const type_map = std.StaticStringMap(TokenType).initComptime(&.{ .{ "int", TokenType.Type }, .{ "float", TokenType.Type }, .{ "double", TokenType.Type }, .{ "void", TokenType.Type }, .{ "byte", TokenType.Type }, .{ "str", TokenType.Type }, .{ "bool", TokenType.Type }, .{ "uint", TokenType.Type }, .{ "type", TokenType.Type }, .{ "ubyte", TokenType.Type }, .{ "usize", TokenType.Type }, .{ "auto", TokenType.Type }, .{ "long", TokenType.Type }, .{ "short", TokenType.Type }, .{ "vec", TokenType.Type }, .{ "map", TokenType.Type } });

fn match_operator(input: []const u8, idx: usize) ?[]const u8 {
    const remaining = input[idx..];
    const max_len = @min(2, remaining.len);

    var len: usize = max_len;
    while (len > 0) {
        const candidate = remaining[0..len];
        if (operator_map.get(candidate)) |_| {
            return candidate;
        }
        len -= 1;
    }
    return null;
}

pub fn lex(input: []const u8, allocator: std.mem.Allocator) ![]Token {
    var tokens = std.ArrayList(Token).init(allocator);
    defer tokens.deinit();

    var idx: usize = 0;
    var line: usize = 1;

    while (idx < input.len) {
        const c = input[idx];

        if (is_whitespace(c)) {
            if (c == '\n') {
                line += 1;
            }
            idx += 1;
            continue;
        } else if (c == '#') {
            while (idx < input.len and input[idx] != '\n') {
                idx += 1;
            }
            continue;
        }

        var token = Token{ .kind = .Unknown, .value = "", .position = idx, .line = line, .is_heap_allocated = false };

        if (match_operator(input, idx)) |op| {
            token.kind = .Operator;
            token.value = op;
            idx += op.len;
            try tokens.append(token);
            continue;
        } else if (is_alpha(c)) {
            var ident_buffer = std.ArrayList(u8).init(allocator);
            defer ident_buffer.deinit();

            while (idx < input.len and (is_alpha(input[idx]) or is_digit(input[idx]))) {
                try ident_buffer.append(input[idx]);
                idx += 1;
            }

            token.kind = .Identifier;
            token.value = try ident_buffer.toOwnedSlice();
            token.is_heap_allocated = true;
        } else if (is_digit(c)) {
            var num_buffer = std.ArrayList(u8).init(allocator);
            defer num_buffer.deinit();

            while (idx < input.len and (is_digit(input[idx]) or input[idx] == '.')) {
                try num_buffer.append(input[idx]);
                idx += 1;
            }

            token.kind = .Number;
            token.value = try num_buffer.toOwnedSlice();
            token.is_heap_allocated = true;
        } else if (c == '"') {
            idx += 1;
            var str_buffer = std.ArrayList(u8).init(allocator);
            defer str_buffer.deinit();

            while (idx < input.len) {
                const current_char = input[idx];

                if (current_char == '\\') {
                    idx += 1;
                    if (idx < input.len) {
                        const next_char = input[idx];
                        switch (next_char) {
                            '\\' => try str_buffer.append('\\'),
                            '"' => try str_buffer.append('"'),
                            'n' => try str_buffer.append('\n'),
                            't' => try str_buffer.append('\t'),
                            'r' => try str_buffer.append('\r'),
                            else => {
                                try str_buffer.append('\\');
                                try str_buffer.append(next_char);
                            },
                        }
                        idx += 1;
                    } else {
                        try str_buffer.append('\\');
                        break;
                    }
                } else if (current_char == '"') {
                    idx += 1;
                    break;
                } else {
                    try str_buffer.append(current_char);
                    idx += 1;
                }
            }

            token.kind = .String;
            token.value = try str_buffer.toOwnedSlice();
            token.is_heap_allocated = true;
        } else if (punctuation_map.has(input[idx .. idx + 1])) {
            const pun = punctuation_map.get(input[idx .. idx + 1]) orelse null;
            if (pun != null) {
                token.kind = .Punctuation;
                token.value = input[idx .. idx + 1];
                idx += 1;
                try tokens.append(token);
                continue;
            }
        } else {
            token.kind = .Unknown;
            token.value = input[idx .. idx + 1];
            idx += 1;
        }

        try tokens.append(token);
    }

    try tokens.append(Token{ .kind = .EOF, .value = "", .position = input.len, .line = line, .is_heap_allocated = false });

    return tokens.toOwnedSlice();
}

pub fn free_tokens(tokens: []Token, allocator: std.mem.Allocator) void {
    for (tokens) |t| {
        if (t.is_heap_allocated) {
            allocator.free(t.value);
        }
    }
    allocator.free(tokens);
}

const LexerError = error{UnkownToken};

pub fn check_tokens(tokens: []Token, allocator: std.mem.Allocator) ![]Token {
    var new_tokens = std.ArrayList(Token).init(allocator);
    for (tokens) |t| {
        if (t.kind == .Unknown) {
            const msg = try std.fmt.allocPrint(allocator, "unknown token \x1b[92m\x1b[3m'{s}'\x1b[22m\x1b[0m", .{t.value});
            const extra = try std.fmt.allocPrint(allocator, "| \x1b[32mAt\x1b[0m: line {} input character {}\n\x1b[0m", .{ t.line, t.position });
            utils.print_error(msg, extra);

            return LexerError.UnkownToken;
        } else if (t.kind == .Identifier) {
            if (keyword_map.has(t.value)) {
                try new_tokens.append(.{ .kind = .Keyword, .value = t.value, .line = t.line, .position = t.position, .is_heap_allocated = t.is_heap_allocated });
                // } else if (type_map.has(t.value)) {
                //     try new_tokens.append(.{ .kind = .Type, .value = t.value, .line = t.line, .position = t.position, .is_heap_allocated = t.is_heap_allocated });
            } else {
                try new_tokens.append(.{ .kind = .Identifier, .value = t.value, .line = t.line, .position = t.position, .is_heap_allocated = t.is_heap_allocated });
            }
        } else {
            try new_tokens.append(t);
        }
    }
    return new_tokens.toOwnedSlice();
}

pub fn print_token(token: Token, idx: usize) void {
    const RESET = "\x1b[0m";
    const RED = "\x1b[31m";
    const GREEN = "\x1b[32m";
    const YELLOW = "\x1b[33m";
    const BLUE = "\x1b[34m";
    const MAGENTA = "\x1b[35m";
    const CYAN = "\x1b[36m";
    const WHITE = "\x1b[37m";

    var kind_color: []const u8 = RESET;
    switch (token.kind) {
        TokenType.Identifier => kind_color = CYAN,
        TokenType.Keyword => kind_color = GREEN,
        TokenType.Type => kind_color = BLUE,
        TokenType.Number => kind_color = YELLOW,
        TokenType.String => kind_color = MAGENTA,
        TokenType.Operator => kind_color = RED,
        TokenType.Punctuation => kind_color = WHITE,
        TokenType.Unknown => kind_color = YELLOW,
        TokenType.EOF => kind_color = BLUE,
        else => kind_color = WHITE,
    }

    std.debug.print("\x1b[1mToken {}:\x1b[0m\n| \x1b[1mKind: {s}{s}\x1b[0m\n| \x1b[1mValue:\x1b[0m \x1b[32m\"{s}\"\x1b[0m\n| \x1b[1mPos:\x1b[0m {d}\n| \x1b[1mLine:\x1b[0m {d}\n", .{ idx, kind_color, @tagName(token.kind), token.value, token.position, token.line });
}

pub fn print_tokens(tokens: []Token) void {
    var idx: usize = 0;
    for (tokens) |t| {
        idx += 1;
        print_token(t, idx);
    }
}
