//
// Created by mafn on 2/14/18.
//

#ifndef HONK_EXPR_HPP
#define HONK_EXPR_HPP

#include <string>
#include <memory>
#include <optional>

#include "Lexer/Token.hpp"
#include "Value.hpp"

// I am sure that somewhere, someone is cursing intently at me as I write this.
// But I can't be fucked to write a unique accept method every time.
#define EXPRVISITOR_ACCEPT(returntype, classname)                \
    returntype accept(ExprVisitor<returntype>& visitor) override \
    {                                                            \
        return visitor.visit_##classname(*this);                 \
    }                                                            \

namespace Honk
{
    template<typename T>
    struct ExprVisitor;

    // Ok, so here's how this is set up:
    //     The Expr class is the base class for all expressions
    //     Different expressions are nested _into_ the base class
    //     Example: Expr::Binary is a binary expression (like 2 * 3)
    struct Expr
    {
        virtual ~Expr() = default;

        virtual std::string accept(ExprVisitor<std::string>& visitor) = 0;
        virtual Value accept(ExprVisitor<Value>& visitor) = 0;

        using u_ptr = std::unique_ptr<Expr>;

        struct Binary;
        struct Grouped;
        struct Literal;
        struct Unary;
        struct VarAccess;
    };

    template<typename T>
    struct ExprVisitor
    {
        virtual T visit_Binary(Expr::Binary& expr) = 0;
        virtual T visit_Grouped(Expr::Grouped& expr) = 0;
        virtual T visit_Literal(Expr::Literal& expr) = 0;
        virtual T visit_Unary(Expr::Unary& expr) = 0;
        virtual T visit_VarAccess(Expr::VarAccess& expr) = 0;
    };

    struct BinaryExprVisitor
    {
        virtual Value visit_minus(const Value& left, const Value& right) = 0;
        virtual Value visit_plus(const Value& left, const Value& right) = 0;
        virtual Value visit_slash(const Value& left, const Value& right) = 0;
        virtual Value visit_star(const Value& left, const Value& right) = 0;
        virtual Value visit_greater(const Value& left, const Value& right) = 0;
        virtual Value visit_greater_eq(const Value& left, const Value& right) = 0;
        virtual Value visit_less_than(const Value& left, const Value& right) = 0;
        virtual Value visit_less_than_eq(const Value& left, const Value& right) = 0;
        virtual Value visit_equals(const Value& left, const Value& right) = 0;
        virtual Value visit_not_equals(const Value& left, const Value& right) = 0;

        using method_ptr = Value (BinaryExprVisitor::*) (const Value& left, const Value& right);
    };

    struct Expr::Binary : Expr
    {
        Binary(Expr::u_ptr left, Token op, Expr::u_ptr right);

        Expr::u_ptr left;
        Token op;
        Expr::u_ptr right;

        TokenType op_type();

        EXPRVISITOR_ACCEPT(std::string, Binary)
        EXPRVISITOR_ACCEPT(Value, Binary)

        Value accept(BinaryExprVisitor& visitor, const Value& left, const Value& right);
    };

    struct Expr::Grouped : Expr
    {
        Grouped(Expr::u_ptr expression);

        Expr::u_ptr expression;

        EXPRVISITOR_ACCEPT(std::string, Grouped)
        EXPRVISITOR_ACCEPT(Value, Grouped)
    };

    struct Expr::Literal : Expr
    {
        Literal(TokenLiteral literal);
        Literal(Value val);

        operator bool() const;

        Value value;

        EXPRVISITOR_ACCEPT(std::string, Literal)
        EXPRVISITOR_ACCEPT(Value, Literal)
    };

    struct Expr::Unary : Expr
    {
        Unary(Token op, Expr::u_ptr right);

        Token op;
        Expr::u_ptr right;

        TokenType op_type();

        EXPRVISITOR_ACCEPT(std::string, Unary)
        EXPRVISITOR_ACCEPT(Value, Unary)
    };

    struct Expr::VarAccess : Expr
    {
        VarAccess(Token identifier);

        std::string get_identifier();

        Token identifier_tok;

        EXPRVISITOR_ACCEPT(std::string, VarAccess)
        EXPRVISITOR_ACCEPT(Value, VarAccess)
    };
}

#endif //HONK_EXPR_HPP
