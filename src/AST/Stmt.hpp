//
// Created by mafn on 2/27/18.
//

#ifndef HONK_STMT_HPP
#define HONK_STMT_HPP

#include <memory>
#include <vector>

#include "Expr.hpp"

#define STMTVISITOR_ACCEPT(returntype, classname)                \
    returntype accept(StmtVisitor<returntype>& visitor) override \
    {                                                            \
        return visitor.visit_##classname(*this);                 \
    }                                                            \

namespace Honk
{
    template<typename T>
    struct StmtVisitor;

    struct Stmt
    {
        virtual ~Stmt() = default;

        virtual void accept(StmtVisitor<void>& visitor) = 0;
        virtual std::string accept(StmtVisitor<std::string>& visitor) = 0;

        using u_ptr = std::unique_ptr<Stmt>;
        using stream = std::vector<u_ptr>;

        struct Expression;
        struct Print;
    };

    template<typename T>
    struct StmtVisitor
    {
        virtual T visit_Expression(Stmt::Expression& stmt) = 0;
        virtual T visit_Print(Stmt::Print& stmt) = 0;
    };

    struct Stmt::Expression : Stmt
    {
        Expression(Expr::u_ptr expression);

        STMTVISITOR_ACCEPT(void, Expression)
        STMTVISITOR_ACCEPT(std::string, Expression)

        Expr::u_ptr expression;
    };

    struct Stmt::Print : Stmt
    {
        Print(Expr::u_ptr expression);

        STMTVISITOR_ACCEPT(void, Print)
        STMTVISITOR_ACCEPT(std::string, Print)

        Expr::u_ptr expression;
    };
}


#endif //HONK_STMT_HPP
