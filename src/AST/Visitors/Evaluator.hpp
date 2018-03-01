//
// Created by mafn on 2/25/18.
//

#ifndef HONK_EVALUATOR_HPP
#define HONK_EVALUATOR_HPP

#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"

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

        // Expression visitor methods
        Value visitLiteral(Expr::Literal& expr) override;
        Value visitGrouped(Expr::Grouped& expr) override;
        Value visitUnary(Expr::Unary& expr) override;
        Value visitBinary(Expr::Binary& expr) override;

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

        Value _evaluate(Expr& expr);
        void _interpret(Stmt& statement);

        bool _is_truthy(const Value& val);
        bool _is_equal(const Value& a, const Value& b);

        // Other helpers
        template <typename T>
        std::pair<T,T> _get_as(const Value& left, const Value& right);
        template <typename T>
        bool _values_are(const Value& left, const Value& right);
        template <typename T>
        std::pair<T, T> _try_get_as(const Value& left, const Value& right, const char* throw_msg);

        // Helper exception throwers
        void _throw_if_not_integer(const Value& left, const Value& right, const char* throw_msg);
    };
}

#endif //HONK_EVALUATOR_HPP
