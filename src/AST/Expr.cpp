//
// Created by mafn on 2/14/18.
//

#include "Expr.hpp"

#include <unordered_map>
#include <functional>

#include "Util/Util.hpp"

namespace Honk
{
    Expr::Expr(const Token& diagnostics_token)
        : diagnostics_token(diagnostics_token)
    {
    }

    Expr::Expr()
        : diagnostics_token(Token::null)
    {
    }

    Expr::Binary::Binary(Expr::u_ptr left, Token op, Expr::u_ptr right)
        : Expr(this->op)
        , left(std::move(left))
        , op(op)
        , right(std::move(right))
    {
    }

    TokenType Expr::Binary::op_type()
    {
        return this->op.type;
    }

    static std::unordered_map<TokenType, BinaryExprVisitor::method_ptr> _accept_dispatch_map = {
        {TokenType::MINUS, &BinaryExprVisitor::visit_minus},
        {TokenType::PLUS, &BinaryExprVisitor::visit_plus},
        {TokenType::SLASH, &BinaryExprVisitor::visit_slash},
        {TokenType::STAR, &BinaryExprVisitor::visit_star},
        {TokenType::GREATER, &BinaryExprVisitor::visit_greater},
        {TokenType::GREATER_EQ, &BinaryExprVisitor::visit_greater_eq},
        {TokenType::LESS_THAN, &BinaryExprVisitor::visit_less_than},
        {TokenType::LESS_THAN_EQ, &BinaryExprVisitor::visit_less_than_eq},
        {TokenType::EQUALS, &BinaryExprVisitor::visit_equals},
        {TokenType::NOT_EQUALS, &BinaryExprVisitor::visit_not_equals},
    };

    Value Expr::Binary::accept(BinaryExprVisitor& visitor, const Value& left, const Value& right)
    {
        std::optional<BinaryExprVisitor::method_ptr> dispatch_method = Util::map_get_optional(_accept_dispatch_map, this->op_type());

        if (!dispatch_method) {
            throw std::runtime_error{"BinaryExprVisitor cannot dispatch the method for this operator"};
        }

        return std::invoke(*dispatch_method, visitor, left, right);
    }

    Expr::Grouped::Grouped(Expr::u_ptr expression)
        : expression(std::move(expression))
    {
    }

    Expr::Literal::Literal(TokenLiteral val)
        : value(Value {val})
    {
    }

    Expr::Literal::Literal(Value val)
        : value(val)
    {
    }

    Expr::Literal::operator bool() const
    {
        return !this->value.is_null();
    }

    Expr::Unary::Unary(Token op, Expr::u_ptr right)
        : Expr(this->op)
        , op(op)
        , right(std::move(right))
    {
    }

    TokenType Expr::Unary::op_type()
    {
        return this->op.type;
    }

    Expr::VarAccess::VarAccess(Token identifier)
        : Expr(this->identifier_tok)
        , identifier_tok(identifier)
    {
    }

    std::string Expr::VarAccess::get_identifier()
    {
        return this->identifier_tok.text;
    }

    Expr::VarAssign::VarAssign(Token identifier_tok, Expr::u_ptr new_value)
        : Expr(this->identifier_tok)
        , identifier_tok(identifier_tok)
        , new_value(std::move(new_value))
    {
    }

    std::string Expr::VarAssign::get_identifier()
    {
        return this->identifier_tok.text;
    }

    Expr::LogicalOr::LogicalOr(Expr::u_ptr left, Expr::u_ptr right)
        : left(std::move(left))
        , right(std::move(right))
    {
    }

    Expr::LogicalAnd::LogicalAnd(Expr::u_ptr left, Expr::u_ptr right)
        : left(std::move(left))
        , right(std::move(right))
    {
    }

    Expr::Call::Call(Expr::u_ptr callee, std::vector<Expr::u_ptr> args)
        : Expr(callee->diagnostics_token)
        , callee(std::move(callee))
        , args(std::move(args))
    {
    }

    Expr::Fun::Fun(std::vector<std::string> parameters, Stmt::u_ptr body)
        : parameters(parameters)
        , body(std::move(body))
    {
    }

    Stmt::Block& Expr::Fun::get_body()
    {
        return *(Stmt::Block*)this->body.get();
    }

    Expr::Get::Get(Expr::u_ptr get_target, Token identifier_tok)
        : Expr(this->identifier_tok)
        , get_target(std::move(get_target))
        , identifier_tok(identifier_tok)
    {
    }

    Expr::Set::Set(Expr::u_ptr set_target, Token identifier_tok, Expr::u_ptr new_value)
        : Expr(this->identifier_tok)
        , set_target(std::move(set_target))
        , identifier_tok(identifier_tok)
        , new_value(std::move(new_value))
    {
    }
}
