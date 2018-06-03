//
// Created by mafn on 2/27/18.
//

#include "Stmt.hpp"

namespace Honk
{
    Stmt::Stmt()
        : diagnostics_token(Token::null)
    {
    }

    Stmt::Stmt(const Token& diagnostics_token)
        : diagnostics_token(diagnostics_token)
    {
    }

    Stmt::Expression::Expression(Expr::u_ptr expression)
        : expression(std::move(expression))
    {
    }

    Stmt::VarDeclaration::VarDeclaration(Token identifier, std::optional<Expr::u_ptr> initializer)
        : Stmt(this->identifier_token)
        , identifier_token(identifier)
        , initializer(std::move(initializer))
    {
    }

    std::string Stmt::VarDeclaration::get_identifier()
    {
        return this->identifier_token.text;
    }

    Stmt::Block::Block(Stmt::stream statements)
        : statements(std::move(statements))
    {
    }

    Stmt::If::If(Expr::u_ptr condition, Stmt::u_ptr true_branch, std::optional<Stmt::u_ptr> false_branch)
        : condition(std::move(condition))
        , true_branch(std::move(true_branch))
        , false_branch(std::move(false_branch))
    {
    }

    Stmt::While::While(Expr::u_ptr condition, Stmt::u_ptr body)
        : condition(std::move(condition))
        , body(std::move(body))
    {
    }

    Stmt::For::For(std::optional<Stmt::u_ptr> init, Expr::u_ptr condition, std::optional<Expr::u_ptr> inc, Stmt::u_ptr body)
        : initializer(std::move(init))
        , condition(std::move(condition))
        , increment(std::move(inc))
        , body(std::move(body))
    {
    }

    Stmt::FunDeclaration::FunDeclaration(Token identifier, std::vector<std::string> parameters, Stmt::u_ptr body)
        : Stmt(this->identifier)
        , identifier(identifier)
        , parameters(parameters)
        , body(std::move(body))
    {
    }

    Stmt::Block& Stmt::FunDeclaration::get_body()
    {
        return *(Stmt::Block*)this->body.get();
    }

    std::string Stmt::FunDeclaration::get_identifier()
    {
        return this->identifier.text;
    }

    Stmt::Return::Return(Token keyword, std::optional<Expr::u_ptr> return_value)
        : keyword(keyword)
        , return_value(std::move(return_value))
    {
    }
}
