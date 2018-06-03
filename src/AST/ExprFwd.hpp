//
// Created by mafn on 5/27/18.
//

#ifndef HONK_EXPRFWD_HPP
#define HONK_EXPRFWD_HPP

#include <string>

#include "Value.hpp"

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

        const Token& diagnostics_token;

        using u_ptr = std::unique_ptr<Expr>;

        struct Binary;
        struct Grouped;
        struct Literal;
        struct LogicalOr;
        struct LogicalAnd;
        struct Unary;
        struct VarAccess;
        struct VarAssign;
        struct Call;

    protected:
        Expr();
        Expr(const Token& diagnostics_token);
    };
}

#endif //HONK_EXPRFWD_HPP