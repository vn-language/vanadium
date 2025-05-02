const lexer = @import("lexer.zig");
const Token = lexer.Token;
const TokenType = lexer.TokenType;

pub const TokenStream = struct {
    tokens: []Token,
    current: usize,

    pub fn init(tokens: []Token) TokenStream {
        return TokenStream{
            .tokens = tokens,
            .current = 0,
        };
    }

    pub fn next(self: *TokenStream) ?Token {
        if (self.current + 1 >= self.tokens.len) return null;
        self.current.* += 1;
        return self.tokens[self.current];
    }

    pub fn get(self: *TokenStream) ?Token {
        if (self.current >= self.tokens.len) return null;
        return self.tokens[self.current];
    }

    pub fn peek(self: *TokenStream, offset: usize) ?Token {
        const offseted = self.current + offset;
        if (offseted >= self.tokens.len) return null;
        return self.tokens[offseted];
    }

    pub fn match(self: *TokenStream, kind: TokenType) bool {
        const token = self.get().?;
        return token != null and token.kind == kind;
    }

    pub fn expect(self: *TokenStream, kind: TokenType) !void {
        const token = self.get().?;
        if (token.kind != null) {
            if (token.kind != kind) {
                return error.UnexpectedToken;
            }
        } else {
            return error.NoToken;
        }
    }
};
