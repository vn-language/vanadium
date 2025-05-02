const std = @import("std");
const lexer = @import("parser/lexer.zig");
const parser = @import("parser/parser.zig");
const ast = @import("parser/ast.zig");
const parser_debug = @import("parser/debug.zig");

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    const source =
        \\ const str motto = "Simple, Transparent, Powerful";
        \\ int x = 5 + 3 * 2;
    ;

    const tokens = try lexer.lex(source, allocator);
    defer lexer.free_tokens(tokens, allocator);

    const checked_tokens = try lexer.check_tokens(tokens, allocator);
    defer allocator.free(checked_tokens);
    lexer.print_tokens(checked_tokens);

    std.debug.print("\n", .{});
}
