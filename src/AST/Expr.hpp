//
// Created by mafn on 2/14/18.
//

#ifndef HONK_EXPR_HPP
#define HONK_EXPR_HPP

#include <string>
#include <memory>
#include <optional>

#include "Lexer/Token.hpp"
#include "Value.hpp"

#include "AST/ExprFwd.hpp"
#include "AST/StmtFwd.hpp"

// I am sure that somewhere, someone is cursing intently at me as I write this.
// But I can't be fucked to write a unique accept method every time.
// TODO: Maybe I can write a DSL for this?
#define EXPRVISITOR_ACCEPT(returntype, classname)                \
    returntype accept(ExprVisitor<returntype>& visitor) override \
    {                                                            \
        return visitor.visit_##classname(*this);                 \
    }                                                            \

#define EXPRVISITORS_ACCEPT(classname)         \
    using u_ptr = std::unique_ptr<classname>;  \
                                               \
    EXPRVISITOR_ACCEPT(std::string, classname) \
    EXPRVISITOR_ACCEPT(Value::s_ptr, classname)\
    EXPRVISITOR_ACCEPT(void, classname)        \

namespace Honk
{
    template<typename T>
    struct ExprVisitor
    {
        virtual T visit_Binary(Expr::Binary& expr) = 0;
        virtual T visit_Grouped(Expr::Grouped& expr) = 0;
        virtual T visit_Literal(Expr::Literal& expr) = 0;
        virtual T visit_LogicalOr(Expr::LogicalOr& expr) = 0;
        virtual T visit_LogicalAnd(Expr::LogicalAnd& expr) = 0;
        virtual T visit_Unary(Expr::Unary& expr) = 0;
        virtual T visit_VarAccess(Expr::VarAccess& expr) = 0;
        virtual T visit_VarAssign(Expr::VarAssign& expr) = 0;
        virtual T visit_Call(Expr::Call& stmt) = 0;
        virtual T visit_Fun(Expr::Fun& expr) = 0;
        virtual T visit_Get(Expr::Get& expr) = 0;
        virtual T visit_Set(Expr::Set& expr) = 0;
        virtual T visit_This(Expr::This& expr) = 0;
    };

    struct BinaryExprVisitor
    {
        virtual Value visit_minus(const Value& left, const Value& right) = 0;
        virtual Value visit_plus(const Value& left, const Value& right) = 0;
        virtual Value visit_slash(const Value& left, const Value& right) = 0;
        virtual Value visit_star(const Value& left, const Value& right) = 0;
        virtual Value visit_greater(const Value& left, const Value& right) = 0;
        virtual Value visit_greater_eq(const Value& left, const Value& right) = 0;
        virtual Value visit_less_than(const Value& left, const Value& right) = 0;
        virtual Value visit_less_than_eq(const Value& left, const Value& right) = 0;
        virtual Value visit_equals(const Value& left, const Value& right) = 0;
        virtual Value visit_not_equals(const Value& left, const Value& right) = 0;

        using method_ptr = Value (BinaryExprVisitor::*) (const Value& left, const Value& right);
    };

    struct Expr::Binary : Expr
    {
        Binary(Expr::u_ptr left, Token op, Expr::u_ptr right);

        Expr::u_ptr left;
        Token op;
        Expr::u_ptr right;

        TokenType op_type();

        EXPRVISITORS_ACCEPT(Binary);

        Value accept(BinaryExprVisitor& visitor, const Value& left, const Value& right);
    };

    struct Expr::Grouped : Expr
    {
        Grouped(Expr::u_ptr expression);

        Expr::u_ptr expression;

        EXPRVISITORS_ACCEPT(Grouped);
    };

    struct Expr::Literal : Expr
    {
        Literal(TokenLiteral literal);
        Literal(Value val);

        operator bool() const;

        Value value;

        EXPRVISITORS_ACCEPT(Literal);
    };

    struct Expr::Unary : Expr
    {
        Unary(Token op, Expr::u_ptr right);

        Token op;
        Expr::u_ptr right;

        TokenType op_type();

        EXPRVISITORS_ACCEPT(Unary);
    };

    struct Expr::VarAccess : Expr
    {
        VarAccess(Token identifier);

        std::string get_identifier();

        Token identifier_tok;

        EXPRVISITORS_ACCEPT(VarAccess);
    };

    // TODO: This get_identifier is a repeated construct
    // Either make the AST not have tokens, or generalise this
    struct Expr::VarAssign : Expr
    {
        VarAssign(Token identifier_tok, Expr::u_ptr new_value);

        std::string get_identifier();

        Token identifier_tok;
        Expr::u_ptr new_value;

        EXPRVISITORS_ACCEPT(VarAssign);
    };

    struct Expr::LogicalOr : Expr
    {
        LogicalOr(Expr::u_ptr left, Expr::u_ptr right);

        Expr::u_ptr left;
        Expr::u_ptr right;

        EXPRVISITORS_ACCEPT(LogicalOr);
    };

    struct Expr::LogicalAnd : Expr
    {
        LogicalAnd(Expr::u_ptr left, Expr::u_ptr right);

        Expr::u_ptr left;
        Expr::u_ptr right;

        EXPRVISITORS_ACCEPT(LogicalAnd);
    };

    struct Expr::Call : Expr
    {
        Call(Expr::u_ptr callee, std::vector<Expr::u_ptr> args);

        Expr::u_ptr callee;
        std::vector<Expr::u_ptr> args;

        EXPRVISITORS_ACCEPT(Call);
    };

    struct Expr::Fun : Expr
    {
        Fun(std::vector<std::string> parameters, Stmt::u_ptr body);

        Stmt::Block& get_body(); // TODO: This is a code smell

        std::vector<std::string> parameters;
        Stmt::u_ptr body;

        EXPRVISITORS_ACCEPT(Fun);
    };

    struct Expr::Get : Expr
    {
        Get(Expr::u_ptr get_target, Token identifier_tok);

        Expr::u_ptr get_target;
        Token identifier_tok;

        EXPRVISITORS_ACCEPT(Get);
    };

    struct Expr::Set : Expr
    {
        Set(Expr::u_ptr set_target, Token identifier_tok, Expr::u_ptr new_value);

        Expr::u_ptr set_target;
        Token identifier_tok;
        Expr::u_ptr new_value;

        EXPRVISITORS_ACCEPT(Set);
    };

    struct Expr::This : Expr
    {
        This(Token this_tok);

        Token this_tok;

        EXPRVISITORS_ACCEPT(This);
    };
}

#endif //HONK_EXPR_HPP
