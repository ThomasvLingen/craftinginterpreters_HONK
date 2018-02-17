//
// Created by mafn on 2/14/18.
//

#ifndef HONK_EXPR_HPP
#define HONK_EXPR_HPP

#include <string>
#include <memory>

#include "Lexer/Token.hpp"

// I am sure that somewhere, someone is cursing intently at me as I write this.
// But I can't be fucked to write a unique accept method every time.
#define VISITOR_ACCEPT(returntype, classname)                \
    returntype accept(Visitor<returntype>& visitor) override \
    {                                                        \
        return visitor.visit##classname(*this);              \
    }                                                        \

namespace Honk
{
    // Forward declarations for visitor
    struct Binary;
    struct Grouped;
    struct Literal;
    struct Unary;

    template <typename T>
    struct Visitor
    {
        virtual T visitBinary(Binary& expr) = 0;
        virtual T visitGrouped(Grouped& expr) = 0;
        virtual T visitLiteral(Literal& expr) = 0;
        virtual T visitUnary(Unary& expr) = 0;
    };

    struct Expr
    {
        virtual ~Expr() = default;

        virtual std::string accept(Visitor<std::string>& visitor) = 0;

        using u_ptr = std::unique_ptr<Expr>;
    };

    struct Binary : Expr
    {
        Binary(Expr::u_ptr left, Token op, Expr::u_ptr right);

        Expr::u_ptr left;
        Token op;
        Expr::u_ptr right;

        VISITOR_ACCEPT(std::string, Binary)
    };

    struct Grouped : Expr
    {
        Grouped(Expr::u_ptr expression);

        Expr::u_ptr expression;

        VISITOR_ACCEPT(std::string, Grouped)
    };

    struct Literal : Expr
    {
        Literal(TokenLiteral val);

        TokenLiteral value;

        VISITOR_ACCEPT(std::string, Literal)
    };

    struct Unary : Expr
    {
        Unary(Token op, Expr::u_ptr right);

        Token op;
        Expr::u_ptr right;

        VISITOR_ACCEPT(std::string, Unary)
    };
}

#endif //HONK_EXPR_HPP
