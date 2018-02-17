//
// Created by mafn on 2/16/18.
//

#ifndef HONK_PRETTYPRINTER_HPP
#define HONK_PRETTYPRINTER_HPP

#include "Expr.hpp"

namespace Honk
{
    struct PrettyASTPrinter : ExprVisitor<std::string>
    {
        void print(Expr& expression);

        std::string visitBinary(Expr::Binary& expr) override;
        std::string visitGrouped(Expr::Grouped& expr) override;
        std::string visitLiteral(Expr::Literal& expr) override;
        std::string visitUnary(Expr::Unary& expr) override;

        std::string parenthesize(const std::string& name, std::initializer_list<Expr*> expressions);
    };
}

#endif //HONK_PRETTYPRINTER_HPP
