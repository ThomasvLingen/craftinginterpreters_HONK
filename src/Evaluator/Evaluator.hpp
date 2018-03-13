//
// Created by mafn on 2/25/18.
//

#ifndef HONK_EVALUATOR_HPP
#define HONK_EVALUATOR_HPP

#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "VariableBucket.hpp"

namespace Honk
{
    struct Interpreter;

    struct Evaluator : ExprVisitor<Value>, BinaryExprVisitor, StmtVisitor<void>
    {
        Evaluator(const Interpreter& parent);

        void interpret(Stmt::stream& code);
        Value evaluate(Expr& expression);

        // Statement visitor methods
        void visit_Expression(Stmt::Expression& stmt) override;
        void visit_Print(Stmt::Print& stmt) override;
        void visit_Block(Stmt::Block& stmt) override;
        void visit_If(Stmt::If& stmt) override;
        void visit_VarDeclaration(Stmt::VarDeclaration& stmt) override;

        // Expression visitor methods
        Value visit_Literal(Expr::Literal& expr) override;
        Value visit_Grouped(Expr::Grouped& expr) override;
        Value visit_Unary(Expr::Unary& expr) override;
        Value visit_VarAccess(Expr::VarAccess& expr) override;
        Value visit_Binary(Expr::Binary& expr) override;
        Value visit_VarAssign(Expr::VarAssign& expr) override;

        // Binary expression visitor methods
        virtual Value visit_minus(const Value& left, const Value& right) override;
        virtual Value visit_plus(const Value& left, const Value& right) override;
        virtual Value visit_slash(const Value& left, const Value& right) override;
        virtual Value visit_star(const Value& left, const Value& right) override;
        virtual Value visit_greater(const Value& left, const Value& right) override;
        virtual Value visit_greater_eq(const Value& left, const Value& right) override;
        virtual Value visit_less_than(const Value& left, const Value& right) override;
        virtual Value visit_less_than_eq(const Value& left, const Value& right) override;
        virtual Value visit_equals(const Value& left, const Value& right) override;
        virtual Value visit_not_equals(const Value& left, const Value& right) override;

    private:
        const Interpreter& _parent;
        VariableBucket::Scoped _scopes;

        Value _evaluate(Expr& expr);
        void _interpret(Stmt& statement);

        bool _is_truthy(const Value& val);
        bool _is_truthy(Expr& expr);
        bool _is_equal(const Value& a, const Value& b);

        // Other helpers
        template <typename T>
        std::pair<T,T> _get_as(const Value& left, const Value& right);
        template <typename T>
        bool _values_are(const Value& left, const Value& right);
        template <typename T>
        std::pair<T, T> _try_get_as(const Value& left, const Value& right, const char* throw_msg);
        VariableBucket& _env();
    };
}

#endif //HONK_EVALUATOR_HPP
