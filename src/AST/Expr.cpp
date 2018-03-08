//
// Created by mafn on 2/14/18.
//

#include "Expr.hpp"

#include <unordered_map>
#include <functional>

#include "Util.hpp"

namespace Honk
{
    Expr::Binary::Binary(Expr::u_ptr left, Token op, Expr::u_ptr right)
        : left(std::move(left))
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
        : op(op)
        , right(std::move(right))
    {
    }

    TokenType Expr::Unary::op_type()
    {
        return this->op.type;
    }

    Expr::VarAccess::VarAccess(Token identifier)
        : identifier_tok(identifier)
    {
    }

    std::string Expr::VarAccess::get_identifier()
    {
        return this->identifier_tok.text;
    }
}
