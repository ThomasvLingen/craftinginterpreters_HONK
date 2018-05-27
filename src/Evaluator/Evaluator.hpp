//
// Created by mafn on 2/25/18.
//

#ifndef HONK_EVALUATOR_HPP
#define HONK_EVALUATOR_HPP

#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "VariableBucket.hpp"
#include "DiagnosticsTokenTracker.hpp"

namespace Honk
{
    struct Interpreter;
    struct StandardLibrary;

    struct EvaluateError : std::runtime_error
    {
        EvaluateError(const char* msg, const Token* error_token);

        const Token* error_token = nullptr;
    };

    struct Evaluator : ExprVisitor<Value>, BinaryExprVisitor, StmtVisitor<void>
    {
        Evaluator(const Interpreter& parent);

        VariableBucket::Scoped scopes;
        VariableBucket& globals = scopes.get_global_env();

        void interpret(Stmt::stream& code);
        Value evaluate(Expr& expression);

        VariableBucket& env();
        void execute_block(Stmt::Block& block);

        // Statement visitor methods
        void visit_Expression(Stmt::Expression& stmt) override;
        void visit_Block(Stmt::Block& stmt) override;
        void visit_If(Stmt::If& stmt) override;
        void visit_VarDeclaration(Stmt::VarDeclaration& stmt) override;
        void visit_While(Stmt::While& stmt) override;
        void visit_For(Stmt::For& stmt) override;
        void visit_FunDeclaration(Stmt::FunDeclaration& stmt) override;

        // Expression visitor methods
        Value visit_Literal(Expr::Literal& expr) override;
        Value visit_LogicalOr(Expr::LogicalOr& expr) override;
        Value visit_LogicalAnd(Expr::LogicalAnd& expr) override;
        Value visit_Grouped(Expr::Grouped& expr) override;
        Value visit_Unary(Expr::Unary& expr) override;
        Value visit_VarAccess(Expr::VarAccess& expr) override;
        Value visit_Binary(Expr::Binary& expr) override;
        Value visit_VarAssign(Expr::VarAssign& expr) override;
        Value visit_Call(Expr::Call& expr) override;

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

        friend class StandardLibrary;
    private:
        const Interpreter& _parent;

        DiagnosticsTokenTracker _callstack;

        Value _evaluate(Expr& expr);
        Value _evaluate_optional(std::optional<Expr::u_ptr>& expr);
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

        EvaluateError _invalid_call_error(size_t expected_n_args, size_t actual);
        EvaluateError _error(const std::string& msg);
    };
}

#endif //HONK_EVALUATOR_HPP
