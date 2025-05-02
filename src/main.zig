const std = @import("std");
const lexer = @import("parser/lexer.zig");
const parser = @import("parser/parser.zig");
const ast = @import("parser/ast.zig");

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    const source =
        \\ mut my_var: str = "Hello, World!";
    ;

    const tokens = try lexer.lex(source, allocator);
    defer lexer.free_tokens(tokens, allocator);

    const checked_tokens = try lexer.check_tokens(tokens, allocator);
    defer allocator.free(checked_tokens);
    lexer.print_tokens(checked_tokens);

    std.debug.print("\n", .{});
}
