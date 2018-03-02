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

    Stmt::VarDeclaration::VarDeclaration(Token identifier, std::optional<Expr::u_ptr> initializer)
        : identifier_token(identifier)
        , initializer(std::move(initializer))
    {
    }

    std::string Stmt::VarDeclaration::get_identifier()
    {
        return this->identifier_token.text;
    }
}
