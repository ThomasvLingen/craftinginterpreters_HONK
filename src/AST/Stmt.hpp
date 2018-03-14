//
// Created by mafn on 2/27/18.
//

#ifndef HONK_STMT_HPP
#define HONK_STMT_HPP

#include <memory>
#include <vector>
#include <optional>

#include "Expr.hpp"

#define STMTVISITOR_ACCEPT(returntype, classname)                \
    returntype accept(StmtVisitor<returntype>& visitor) override \
    {                                                            \
        return visitor.visit_##classname(*this);                 \
    }                                                            \

#define STMTVISITORS_ACCEPT(classname)         \
    STMTVISITOR_ACCEPT(void       , classname) \
    STMTVISITOR_ACCEPT(std::string, classname) \

namespace Honk
{
    template<typename T>
    struct StmtVisitor;

    struct Stmt
    {
        virtual ~Stmt() = default;

        virtual void accept(StmtVisitor<void>& visitor) = 0;
        virtual std::string accept(StmtVisitor<std::string>& visitor) = 0;

        const Token& diagnostics_token;

        using u_ptr = std::unique_ptr<Stmt>;
        using stream = std::vector<u_ptr>;

        struct Expression;
        struct Print;
        struct Block;
        struct VarDeclaration;
        struct If;
        struct While;

    private:
        Stmt();
        Stmt(const Token& diagnostics_token);
    };

    template<typename T>
    struct StmtVisitor
    {
        virtual T visit_Expression(Stmt::Expression& stmt) = 0;
        virtual T visit_Print(Stmt::Print& stmt) = 0;
        virtual T visit_Block(Stmt::Block& stmt) = 0;
        virtual T visit_If(Stmt::If& stmt) = 0;
        virtual T visit_VarDeclaration(Stmt::VarDeclaration& stmt) = 0;
        virtual T visit_While(Stmt::While& stmt) = 0;
    };

    struct Stmt::Expression : Stmt
    {
        Expression(Expr::u_ptr expression);

        Expr::u_ptr expression;

        STMTVISITORS_ACCEPT(Expression)
    };

    struct Stmt::Print : Stmt
    {
        Print(Expr::u_ptr expression);

        Expr::u_ptr expression;

        STMTVISITORS_ACCEPT(Print)
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
}


#endif //HONK_STMT_HPP
