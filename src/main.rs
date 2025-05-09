use chumsky::Parser;
pub mod parser;

fn main() {
    let src = "2 + 2 - (3 * 9 / 7)";
    match parser::parser().parse(src).into_result() {
        Ok(ast) => println!("{:#?}", ast),
        Err(parse_errs) => parse_errs
            .into_iter()
            .for_each(|e| println!("Parse error: {}", e)),
    }
}
