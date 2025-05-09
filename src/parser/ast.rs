#[derive(Debug, PartialEq)]
pub enum Node<'src> {
    Int(i64),
    Float(f64),
    Var(&'src str),
    Not(Box<Node<'src>>),
    VarDecl {
        name: &'src str,
        value: Box<Node<'src>>,
    },

    Add(Box<Node<'src>>, Box<Node<'src>>),
    Sub(Box<Node<'src>>, Box<Node<'src>>),
    Mul(Box<Node<'src>>, Box<Node<'src>>),
    Div(Box<Node<'src>>, Box<Node<'src>>),
    Call(&'src str, Vec<Node<'src>>),
}
