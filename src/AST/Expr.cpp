//
// Created by mafn on 2/14/18.
//

#include "Expr.hpp"

namespace Honk
{
    Expr::Binary::Binary(Expr::u_ptr left, Token op, Expr::u_ptr right)
        : left(std::move(left))
        , op(op)
        , right(std::move(right))
    {
    }

    Expr::Grouped::Grouped(Expr::u_ptr expression)
        : expression(std::move(expression))
    {
    }

    Expr::Literal::Literal(TokenLiteral val)
        : value(val)
    {
    }

    Expr::Literal::Literal()
        : value(std::nullopt)
    {
    }

    Expr::Literal::operator bool() const
    {
        return this->value.has_value();
    }

    Expr::Unary::Unary(Token op, Expr::u_ptr right)
        : op(op)
        , right(std::move(right))
    {
    }
}
