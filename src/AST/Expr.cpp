//
// Created by mafn on 2/14/18.
//

#include "Expr.hpp"

namespace Honk
{
    Binary::Binary(Expr::u_ptr left, Token op, Expr::u_ptr right)
        : left(std::move(left))
        , op(op)
        , right(std::move(right))
    {
    }

    Grouped::Grouped(Expr::u_ptr expression)
        : expression(std::move(expression))
    {
    }

    Literal::Literal(TokenLiteral val)
        : value(val)
    {
    }

    Unary::Unary(Token op, Expr::u_ptr right)
        : op(op)
        , right(std::move(right))
    {
    }
}
