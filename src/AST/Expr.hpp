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
    {                                                        \
        return visitor.visit##classname(*this);              \
    }                                                        \

namespace Honk
{
    template <typename T>
    struct ExprVisitor;

    // Ok, so here's how this is set up:
    //     The Expr class is the base class for all expressions
    //     Different expressions are nested _into_ the base class
    //     Example: Expr::Binary is a binary expression (like 2 * 3)
    struct Expr
    {
        virtual ~Expr() = default;

        virtual std::string accept(ExprVisitor<std::string>& visitor) = 0;

        using u_ptr = std::unique_ptr<Expr>;

        struct Binary;
        struct Grouped;
        struct Literal;
        struct Unary;
    };

    template <typename T>
    struct ExprVisitor
    {
        virtual T visitBinary(Expr::Binary& expr) = 0;
        virtual T visitGrouped(Expr::Grouped& expr) = 0;
        virtual T visitLiteral(Expr::Literal& expr) = 0;
        virtual T visitUnary(Expr::Unary& expr) = 0;
    };

    struct Expr::Binary : Expr
    {
        Binary(Expr::u_ptr left, Token op, Expr::u_ptr right);

        Expr::u_ptr left;
        Token op;
        Expr::u_ptr right;

        EXPRVISITOR_ACCEPT(std::string, Binary)
    };

    struct Expr::Grouped : Expr
    {
        Grouped(Expr::u_ptr expression);

        Expr::u_ptr expression;

        EXPRVISITOR_ACCEPT(std::string, Grouped)
    };

    struct Expr::Literal : Expr
    {
        Literal(TokenLiteral literal);
        Literal(Value val);

        operator bool() const;

        Value value;

        EXPRVISITOR_ACCEPT(std::string, Literal)
    };

    struct Expr::Unary : Expr
    {
        Unary(Token op, Expr::u_ptr right);

        Token op;
        Expr::u_ptr right;

        EXPRVISITOR_ACCEPT(std::string, Unary)
    };
}

#endif //HONK_EXPR_HPP
