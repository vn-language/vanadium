const std = @import("std");

pub fn unused(T: type, v: T) T {
    return v;
}

pub fn print_error(msg: []const u8, extra: []const u8) void {
    std.debug.print("\x1b[1;31mVanadium: error\x1b[0m\n", .{});
    std.debug.print("| \x1b[34mMessage\x1b[0m: {s}\n", .{msg});
    if (!std.mem.eql(u8, extra, "")) {
        std.debug.print("{s}\n", .{extra});
    }
}
