//
// Created by mafn on 2/27/18.
//

#include "Stmt.hpp"

namespace Honk
{
    Stmt::Expression::Expression(Expr::u_ptr expression)
        : expression(std::move(expression))
    {
    }

    Stmt::Print::Print(Expr::u_ptr expression)
        : expression(std::move(expression))
    {
    }
}
