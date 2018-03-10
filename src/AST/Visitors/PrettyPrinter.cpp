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

    void PrettyASTPrinter::print(Stmt& statement)
    {
        std::cout << statement.accept(*this) << std::endl;
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

    std::string PrettyASTPrinter::visit_Binary(Expr::Binary& expr)
    {
        return parenthesize(expr.op.text, {expr.left.get(), expr.right.get()});
    }

    std::string PrettyASTPrinter::visit_Grouped(Expr::Grouped& expr)
    {
        return parenthesize("group", {expr.expression.get()});
    }

    std::string PrettyASTPrinter::visit_Literal(Expr::Literal& expr)
    {
        return expr.value.to_str();
    }

    std::string PrettyASTPrinter::visit_Unary(Expr::Unary& expr)
    {
        return parenthesize(expr.op.text, {expr.right.get()});
    }

    std::string PrettyASTPrinter::visit_VarAccess(Expr::VarAccess& expr)
    {
        return expr.get_identifier();
    }

    std::string PrettyASTPrinter::visit_Expression(Stmt::Expression& stmt)
    {
        std::string output;
        output += "[expr ";
        output += stmt.expression->accept(*this);
        output += "]";

        return output;
    }

    std::string PrettyASTPrinter::visit_Print(Stmt::Print& stmt)
    {
        std::string output;
        output += "[print ";
        output += stmt.expression->accept(*this);
        output += "]";

        return output;
    }

    std::string PrettyASTPrinter::visit_VarDeclaration(Stmt::VarDeclaration& stmt)
    {
        std::string output;
        output += "[var_decl ";
        output += stmt.get_identifier();

        if (stmt.initializer) {
            Expr& initializer = **stmt.initializer;
            output += " = ";
            output += initializer.accept(*this);
        }

        output += "]";

        return output;
    }

    std::string PrettyASTPrinter::visit_VarAssign(Expr::VarAssign& expr)
    {
        return parenthesize(expr.identifier_tok.text + "=", {expr.new_value.get()});
    }
}
