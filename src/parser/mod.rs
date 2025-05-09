use chumsky::prelude::*;

pub mod ast;
use ast::Node;

fn expr_parser<'a>() -> impl Parser<'a, &'a str, Node<'a>> {
    let ident = text::ascii::ident().padded();

    recursive(|expr| {
        let int = text::int(10).map(|s: &str| Node::Int(s.parse().unwrap()));

        let call = ident
            .then(
                expr.clone()
                    .separated_by(just(','))
                    .allow_trailing()
                    .collect::<Vec<_>>()
                    .delimited_by(just('('), just(')')),
            )
            .map(|(f, args)| Node::Call(f, args));

        let atom = int
            .or(expr.delimited_by(just('('), just(')')))
            .or(call)
            .or(ident.map(Node::Var))
            .padded();

        let op = |c| just(c).padded();

        let unary = op('-')
            .repeated()
            .foldr(atom, |_op, rhs| Node::Not(Box::new(rhs)));

        let product = unary.clone().foldl(
            choice((
                op('*').to(Node::Mul as fn(_, _) -> _),
                op('/').to(Node::Div as fn(_, _) -> _),
            ))
            .then(unary)
            .repeated(),
            |lhs, (op, rhs)| op(Box::new(lhs), Box::new(rhs)),
        );

        let sum = product.clone().foldl(
            choice((
                op('+').to(Node::Add as fn(_, _) -> _),
                op('-').to(Node::Sub as fn(_, _) -> _),
            ))
            .then(product)
            .repeated(),
            |lhs, (op, rhs)| op(Box::new(lhs), Box::new(rhs)),
        );

        sum
    })
}

pub fn parser<'a>() -> impl Parser<'a, &'a str, Node<'a>> {
    expr_parser()
}
