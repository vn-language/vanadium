use chumsky::Parser;
pub mod parser;

fn main() {
    let src = "
    imut name: str = \"Jhon Doe\";
    io::print(name);
    \\
    ";
    match parser::lexer().parse(src).into_result() {
        Ok(tokens) => {
            for token in tokens {
                println!("{:?}", token);
            }
        }
        Err(parse_errs) => parse_errs
            .into_iter()
            .for_each(|e| println!("Parse error: {}", e)),
    }
}
