//
// Created by mafn on 2/16/18.
//

#include "PrettyPrinter.hpp"

#include <iostream>
#include <sstream>

namespace Honk
{
    void PrettyASTPrinter::print(Expr& expression)
    {
        std::cout << expression.accept(*this) << std::endl;
    }

    std::string PrettyASTPrinter::parenthesize(const std::string& name, std::initializer_list<Expr*> expressions)
    {
        std::stringstream output;

        output << "(" << name;
        for (Expr* expr : expressions) {
            output << " " << expr->accept(*this);
        }
        output << ")";

        return output.str();
    }

    std::string PrettyASTPrinter::visitBinary(Binary& expr)
    {
        return parenthesize(expr.op.text, {expr.left.get(), expr.right.get()});
    }

    std::string PrettyASTPrinter::visitGrouped(Grouped& expr)
    {
        return parenthesize("group", {expr.expression.get()});
    }

    std::string PrettyASTPrinter::visitLiteral(Literal& expr)
    {
        return Token::literal_to_str(expr.value);
    }

    std::string PrettyASTPrinter::visitUnary(Unary& expr)
    {
        return parenthesize(expr.op.text, {expr.right.get()});
    }
}
