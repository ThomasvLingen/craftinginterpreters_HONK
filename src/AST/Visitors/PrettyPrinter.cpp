//
// Created by mafn on 2/16/18.
//

#include "PrettyPrinter.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>

#include "Util/Util.hpp"

namespace Honk
{
    // TODO: The code for this class is REALLY dupey.
    // Consider rewriting it.
    void PrettyASTPrinter::print(Expr& expression)
    {
        std::cout << expression.accept(*this) << std::endl;
    }

    void PrettyASTPrinter::print(Stmt& statement)
    {
        std::cout << statement.accept(*this) << std::endl;
    }

    std::string PrettyASTPrinter::parenthesize(const std::string& name, std::vector<Expr::u_ptr>& exprs)
    {
        std::vector<Expr*> expr_ptrs = Util::map<std::vector<Expr*>>(exprs,
            [] (Expr::u_ptr& arg) {
                return arg.get();
            });

        return this->parenthesize(name, expr_ptrs);
    }

    std::string PrettyASTPrinter::parenthesize(const std::string& name, std::vector<Expr*> expressions)
    {
        std::vector<std::string> expr_strings = Util::map<std::vector<std::string>>(expressions,
            [this] (Expr* expr) {
                return expr->accept(*this);
            });

        return this->parenthesize(name, expr_strings);
    }

    std::string PrettyASTPrinter::parenthesize(const std::string& name, std::vector<std::string>& strings)
    {
        std::stringstream output;

        output << "(" << name;
        for (std::string& str : strings) {
            output << " " << str;
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

    std::string PrettyASTPrinter::visit_Block(Stmt::Block& stmt)
    {
        std::string output;

        output += "{\n";
        for (Stmt::u_ptr& statement : stmt.statements) {
            output += statement->accept(*this) + "\n";
        }
        output += "}";
        return output;
    }

    std::string PrettyASTPrinter::visit_If(Stmt::If& stmt)
    {
        std::string output;
        output += "[if ";
        output += stmt.condition->accept(*this) + " ";
        output += stmt.true_branch->accept(*this);

        if (stmt.false_branch) {
            Stmt& false_branch = **stmt.false_branch;
            output += " else " + false_branch.accept(*this);
        }

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

    std::string PrettyASTPrinter::visit_FunDeclaration(Stmt::FunDeclaration& stmt)
    {
        std::string output;
        output += "[fun=";
        output += Token::get_text(stmt.identifier);
        output += this->_to_str(*stmt.function);
        output += "]";

        return output;
    }

    std::string PrettyASTPrinter::visit_While(Stmt::While& stmt)
    {
        std::string output;
        output += "[while ";
        output += stmt.condition->accept(*this) + " ";
        output += stmt.body->accept(*this);
        output += "]";

        return output;
    }

    std::string PrettyASTPrinter::visit_For(Stmt::For& stmt)
    {
        std::string output;
        output += "[for ";

        if (stmt.initializer) {
            output += "init = " + this->_to_str(**stmt.initializer) + " ";
        }

        output += "cond = " + this->_to_str(*stmt.condition) + " ";

        if (stmt.increment) {
            output += "inc = " + this->_to_str(**stmt.increment) + " ";
        }

        output += "]";
        return output;
    }

    std::string PrettyASTPrinter::visit_Return(Stmt::Return& stmt)
    {
        std::string output;
        output += "[return ";
        if (stmt.return_value) {
            output += this->_to_str(**stmt.return_value);
        }
        output += "]";
        return output;
    }

    std::string PrettyASTPrinter::visit_Class(Stmt::Class& stmt)
    {
        std::string output;
        output += "[class ";
        output +=  stmt.name.text;
        output += "\n";

        for (Stmt::FunDeclaration::u_ptr& method : stmt.methods) {
            output += this->_to_str(*method);
        }

        output += "]";

        return output;
    }

    std::string PrettyASTPrinter::visit_VarAssign(Expr::VarAssign& expr)
    {
        return parenthesize(expr.identifier_tok.text + "=", {expr.new_value.get()});
    }

    std::string PrettyASTPrinter::visit_LogicalOr(Expr::LogicalOr& expr)
    {
        return parenthesize("or", {expr.left.get(), expr.right.get()});
    }

    std::string PrettyASTPrinter::visit_LogicalAnd(Expr::LogicalAnd& expr)
    {
        return parenthesize("and", {expr.left.get(), expr.right.get()});
    }

    std::string PrettyASTPrinter::visit_Call(Expr::Call& expr)
    {
        std::string output;
        output += "[call=";
        output += this->_to_str(*expr.callee);
        if (expr.args.size() > 0) {
            output += " " + parenthesize("args", expr.args);
        }
        output += "]";

        return output;
    }


    std::string PrettyASTPrinter::visit_Fun(Expr::Fun& expr)
    {
        std::string output;
        output += "(fun";

        if (expr.parameters.size() > 0) {
            output += " " + this->parenthesize("params", expr.parameters);
        }

        output += expr.body->accept(*this);

        return output;
    }

    std::string PrettyASTPrinter::visit_Get(Expr::Get& expr)
    {
        std::string output;
        output += "(get ";
        output += _to_str(*expr.get_target);
        output += "." + Token::get_text(expr.identifier_tok);
        output += ")";

        return output;
    }

    std::string PrettyASTPrinter::_to_str(Stmt& stmt)
    {
        return stmt.accept(*this);
    }

    std::string PrettyASTPrinter::_to_str(Expr& expr)
    {
        return expr.accept(*this);
    }

    std::string PrettyASTPrinter::visit_Set(Expr::Set& expr)
    {
        std::string output;
        output += "(set ";
        output += _to_str(*expr.set_target);
        output += "<" + Token::get_text(expr.identifier_tok) + ">";
        output += " = " + _to_str(*expr.new_value);
        output += ")";

        return output;
    }
}
