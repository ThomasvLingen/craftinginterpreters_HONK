//
// Created by mafn on 2/16/18.
//

#ifndef HONK_PRETTYPRINTER_HPP
#define HONK_PRETTYPRINTER_HPP

#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"

namespace Honk
{
    struct PrettyASTPrinter : ExprVisitor<std::string>, StmtVisitor<std::string>
    {
        void print(Expr& expression);
        void print(Stmt& statement);

        // Statement visitors
        std::string visit_Expression(Stmt::Expression& stmt) override;
        std::string visit_Block(Stmt::Block& stmt) override;
        std::string visit_If(Stmt::If& stmt) override;
        std::string visit_VarDeclaration(Stmt::VarDeclaration& stmt) override;
        std::string visit_FunDeclaration(Stmt::FunDeclaration& stmt) override;
        std::string visit_While(Stmt::While& stmt) override;
        std::string visit_For(Stmt::For& stmt) override;
        std::string visit_Return(Stmt::Return& stmt) override;
        std::string visit_Class(Stmt::Class& stmt) override;

        // Expression visitors
        std::string visit_Binary(Expr::Binary& expr) override;
        std::string visit_Call(Expr::Call& expr) override;
        std::string visit_Grouped(Expr::Grouped& expr) override;
        std::string visit_Literal(Expr::Literal& expr) override;
        std::string visit_LogicalOr(Expr::LogicalOr& expr) override;
        std::string visit_LogicalAnd(Expr::LogicalAnd& expr) override;
        std::string visit_Unary(Expr::Unary& expr) override;
        std::string visit_VarAccess(Expr::VarAccess& expr) override;
        std::string visit_VarAssign(Expr::VarAssign& expr) override;
        std::string visit_Fun(Expr::Fun& expr) override;
        std::string visit_Get(Expr::Get& expr) override;
        std::string visit_Set(Expr::Set& expr) override;
        std::string visit_This(Expr::This& expr) override;

        std::string parenthesize(const std::string& name, std::vector<Expr*> expressions);
        std::string parenthesize(const std::string& name, std::vector<Expr::u_ptr>& exprs);
        std::string parenthesize(const std::string& name, std::vector<std::string>& strs);

    private:
        std::string _to_str(Stmt& stmt);
        std::string _to_str(Expr& expr);
    };
}

#endif //HONK_PRETTYPRINTER_HPP
