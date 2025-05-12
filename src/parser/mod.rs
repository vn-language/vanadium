use ariadne::{sources, Color, Label, Report, ReportKind};
use chumsky::{input::ValueInput, prelude::*};
use std::{collections::HashMap, env, fmt, fs};

pub mod ast;
use ast::Node;

pub type Span = SimpleSpan;
pub type Spanned<T> = (T, Span);

#[derive(Clone, Debug, PartialEq)]
pub enum Type {
    I16,
    I32,
    I64,
    U8,
    U16,
    U32,
    U64,
    STR,
}

#[derive(Clone, Debug, PartialEq)]
pub enum Token<'src> {
    Null,
    Bool(bool),
    Num(f64),
    Str(&'src str),
    Op(&'src str),
    Ctrl(char),
    Ident(&'src str),
    Type(Box<Type>),

    If,
    Else,
    Elif,
    While,
    For,
    Until,
    Include,
    Defer,
    Delete,
    Switch,
    Case,
    Default,
    Fn,
    Return,
    Struct,
    New,
    //Macro,
    Interface,
    Using,
    Enum,
    Mut,
    Imut,
    Unsafe,
    In,
    Break,
    Continue,
    As,
    Publ,
    Shared,
}

impl fmt::Display for Token<'_> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Token::Null => write!(f, "null"),
            Token::Bool(x) => write!(f, "{}", x),
            Token::Num(n) => write!(f, "{}", n),
            Token::Str(s) => write!(f, "{}", s),
            Token::Op(s) => write!(f, "{}", s),
            Token::Ctrl(c) => write!(f, "{}", c),
            Token::Ident(s) => write!(f, "{}", s),
            Token::If => write!(f, "if"),
            Token::Else => write!(f, "else"),
            Token::Elif => write!(f, "elif"),
            Token::While => write!(f, "while"),
            Token::For => write!(f, "for"),
            Token::Until => write!(f, "until"),
            Token::Include => write!(f, "include"),
            Token::Defer => write!(f, "defer"),
            Token::Delete => write!(f, "delete"),
            Token::Switch => write!(f, "switch"),
            Token::Case => write!(f, "case"),
            Token::Default => write!(f, "default"),
            Token::Fn => write!(f, "fn"),
            Token::Return => write!(f, "return"),
            Token::Struct => write!(f, "struct"),
            Token::New => write!(f, "new"),
            //Token::Macro => write!(f, "macro"),
            Token::Interface => write!(f, "interface"),
            Token::Using => write!(f, "using"),
            Token::Enum => write!(f, "enum"),
            Token::Mut => write!(f, "mut"),
            Token::Imut => write!(f, "imut"),
            Token::Unsafe => write!(f, "unsafe"),
            Token::In => write!(f, "in"),
            Token::Break => write!(f, "break"),
            Token::Continue => write!(f, "continue"),
            Token::As => write!(f, "as"),
            Token::Publ => write!(f, "publ"),
            Token::Shared => write!(f, "shared"),
            Token::Type(t) => match **t {
                Type::I16 => write!(f, "int16"),
                Type::I32 => write!(f, "int32"),
                Type::I64 => write!(f, "int64"),
                Type::U8 => write!(f, "uint8"),
                Type::U16 => write!(f, "uint16"),
                Type::U32 => write!(f, "uint32"),
                Type::U64 => write!(f, "uint64"),
                Type::STR => write!(f, "str"),
                //_ => write!(f, ""),
            },
        }
    }
}

fn box_type(t: Type) -> crate::parser::Token<'static> {
    Token::Type(Box::new(t))
}

pub fn lexer<'src>(
) -> impl Parser<'src, &'src str, Vec<Spanned<Token<'src>>>, extra::Err<Rich<'src, char, Span>>> {
    let num = text::int(10)
        .then(just('.').then(text::digits(10)).or_not())
        .to_slice()
        .from_str()
        .unwrapped()
        .map(Token::Num);

    let str_ = just('"')
        .ignore_then(none_of('"').repeated().to_slice())
        .then_ignore(just('"'))
        .map(Token::Str);

    let op = one_of("+*-/!=")
        .repeated()
        .at_least(1)
        .to_slice()
        .map(Token::Op);

    let ctrl = one_of("()[]{}:;,").map(Token::Ctrl);

    let ident = text::ascii::ident().map(|ident: &str| match ident {
        "if" => Token::If,
        "else" => Token::Else,
        "elif" => Token::Elif,
        "while" => Token::While,
        "for" => Token::For,
        "until" => Token::Until,
        "include" => Token::Include,
        "defer" => Token::Defer,
        "delete" => Token::Delete,
        "switch" => Token::Switch,
        "case" => Token::Case,
        "default" => Token::Default,
        "fn" => Token::Fn,
        "return" => Token::Return,
        "struct" => Token::Struct,
        "new" => Token::New,
        "interface" => Token::Interface,
        "using" => Token::Using,
        "enum" => Token::Enum,
        "mut" => Token::Mut,
        "imut" => Token::Imut,
        "unsafe" => Token::Unsafe,
        "true" => Token::Bool(true),
        "false" => Token::Bool(false),
        "none" => Token::Null,
        "int16" => box_type(Type::I16),
        "int32" => box_type(Type::I32),
        "int64" => box_type(Type::I64),
        "uint8" => box_type(Type::U8),
        "uint16" => box_type(Type::U16),
        "uint32" => box_type(Type::U32),
        "uint64" => box_type(Type::U64),
        "str" => box_type(Type::STR),
        _ => Token::Ident(ident),
    });

    let token = num.or(str_).or(op).or(ctrl).or(ident);

    let comment = just("@")
        .then(any().and_is(just('\n').not()).repeated())
        .padded();

    token
        .map_with(|tok, e| (tok, e.span()))
        .padded_by(comment.repeated())
        .padded()
        .recover_with(skip_then_retry_until(any().ignored(), end()))
        .repeated()
        .collect()
}

//pub fn parser<'a>() -> impl Parser<'a, &'a str, Node<'a>, extra::Err<Rich<'a, char>>> {
//}
