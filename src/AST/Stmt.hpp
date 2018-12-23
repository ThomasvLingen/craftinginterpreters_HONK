//
// Created by mafn on 2/27/18.
//

#ifndef HONK_STMT_HPP
#define HONK_STMT_HPP

#include <memory>
#include <vector>
#include <optional>

#include "Expr.hpp"
#include "StmtFwd.hpp"

#define STMTVISITOR_ACCEPT(returntype, classname)                \
    returntype accept(StmtVisitor<returntype>& visitor) override \
    {                                                            \
        return visitor.visit_##classname(*this);                 \
    }                                                            \

#define STMTVISITORS_ACCEPT(classname)         \
    using u_ptr = std::unique_ptr<classname>;  \
                                               \
    STMTVISITOR_ACCEPT(void       , classname) \
    STMTVISITOR_ACCEPT(std::string, classname) \

namespace Honk
{
    template<typename T>
    struct StmtVisitor
    {
        virtual T visit_Expression(Stmt::Expression& stmt) = 0;
        virtual T visit_Block(Stmt::Block& stmt) = 0;
        virtual T visit_If(Stmt::If& stmt) = 0;
        virtual T visit_VarDeclaration(Stmt::VarDeclaration& stmt) = 0;
        virtual T visit_While(Stmt::While& stmt) = 0;
        virtual T visit_For(Stmt::For& stmt) = 0;
        virtual T visit_FunDeclaration(Stmt::FunDeclaration& stmt) = 0;
        virtual T visit_Return(Stmt::Return& stmt) = 0;
        virtual T visit_Class(Stmt::Class& stmt) = 0;
    };

    struct Stmt::Expression : Stmt
    {
        Expression(Expr::u_ptr expression);

        Expr::u_ptr expression;

        STMTVISITORS_ACCEPT(Expression)
    };

    struct Stmt::Block : Stmt
    {
        Block(Stmt::stream statements);

        Stmt::stream statements;

        STMTVISITORS_ACCEPT(Block)
    };

    struct Stmt::If : Stmt
    {
        If(Expr::u_ptr condition, Stmt::u_ptr true_branch, std::optional<Stmt::u_ptr> false_branch);

        Expr::u_ptr condition;
        Stmt::u_ptr true_branch;
        std::optional<Stmt::u_ptr> false_branch;

        STMTVISITORS_ACCEPT(If)
    };

    struct Stmt::VarDeclaration : Stmt
    {
        VarDeclaration(Token identifier, std::optional<Expr::u_ptr> initializer);

        std::string get_identifier();

        Token identifier_token;
        std::optional<Expr::u_ptr> initializer;

        STMTVISITORS_ACCEPT(VarDeclaration)
    };

    struct Stmt::While : Stmt
    {
        While(Expr::u_ptr condition, Stmt::u_ptr body);

        Expr::u_ptr condition;
        Stmt::u_ptr body;

        STMTVISITORS_ACCEPT(While)
    };

    struct Stmt::For : Stmt
    {
        For(std::optional<Stmt::u_ptr> init, Expr::u_ptr condition, std::optional<Expr::u_ptr> inc, Stmt::u_ptr body);

        std::optional<Stmt::u_ptr> initializer;
        Expr::u_ptr condition;
        std::optional<Expr::u_ptr> increment;

        Stmt::u_ptr body;

        STMTVISITORS_ACCEPT(For);
    };

    struct Stmt::FunDeclaration : Stmt
    {
        FunDeclaration(Token identifier, Expr::u_ptr function);

        Expr::Fun& get_fun();
        std::string get_identifier();

        Token identifier;
        Expr::u_ptr function;

        STMTVISITORS_ACCEPT(FunDeclaration);
    };

    struct Stmt::Return : Stmt
    {
        Return(Token keyword, std::optional<Expr::u_ptr> return_value);

        Token keyword;
        std::optional<Expr::u_ptr> return_value;

        STMTVISITORS_ACCEPT(Return);
    };

    struct Stmt::Class : Stmt
    {
        Class(Token name, std::vector<Stmt::VarDeclaration::u_ptr> fields, std::vector<Stmt::FunDeclaration::u_ptr> methods);

        Token name;
        std::vector<Stmt::VarDeclaration::u_ptr> fields;
        std::vector<Stmt::FunDeclaration::u_ptr> methods;

        STMTVISITORS_ACCEPT(Class);
    };
}


#endif //HONK_STMT_HPP
