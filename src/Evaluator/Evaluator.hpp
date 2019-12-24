//
// Created by mafn on 2/25/18.
//

#ifndef HONK_EVALUATOR_HPP
#define HONK_EVALUATOR_HPP

#include "Errors.hpp"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "Resolver/Resolver.hpp"
#include "VariableBucket.hpp"
#include "DiagnosticsTokenTracker.hpp"

namespace Honk
{
    struct Interpreter;
    struct StandardLibrary;

    struct Return : std::exception
    {
        Return(Value returned_value);

        Value returned_value;
    };

    struct Evaluator : ExprVisitor<Value::s_ptr>, BinaryExprVisitor, StmtVisitor<void>
    {
        Evaluator(const Interpreter& parent);

        VariableBucket::Scoped scopes;
        VariableBucket& globals = scopes.get_global_env();

        void interpret(Stmt::stream& code);
        Value evaluate(Expr& expression);

        void add_resolves(const VariableResolveMapping& to_add);

        VariableBucket& env();
        VariableBucket& env(size_t indirections);
        VariableBucket& get_target_env(const Expr& access_expr);
        VariableBucket::s_ptr claim_env();

        void execute_block(Stmt::Block& block);

        // Statement visitor methods
        void visit_Expression(Stmt::Expression& stmt) override;
        void visit_Block(Stmt::Block& stmt) override;
        void visit_If(Stmt::If& stmt) override;
        void visit_VarDeclaration(Stmt::VarDeclaration& stmt) override;
        void visit_While(Stmt::While& stmt) override;
        void visit_For(Stmt::For& stmt) override;
        void visit_FunDeclaration(Stmt::FunDeclaration& stmt) override;
        void visit_Return(Stmt::Return& stmt) override;
        void visit_Class(Stmt::Class& stmt) override;

        // Expression visitor methods
        Value::s_ptr visit_Literal(Expr::Literal& expr) override;
        Value::s_ptr visit_LogicalOr(Expr::LogicalOr& expr) override;
        Value::s_ptr visit_LogicalAnd(Expr::LogicalAnd& expr) override;
        Value::s_ptr visit_Grouped(Expr::Grouped& expr) override;
        Value::s_ptr visit_Unary(Expr::Unary& expr) override;
        Value::s_ptr visit_VarAccess(Expr::VarAccess& expr) override;
        Value::s_ptr visit_Binary(Expr::Binary& expr) override;
        Value::s_ptr visit_VarAssign(Expr::VarAssign& expr) override;
        Value::s_ptr visit_Call(Expr::Call& expr) override;
        Value::s_ptr visit_Fun(Expr::Fun& expr) override;
        Value::s_ptr visit_Get(Expr::Get& expr) override;
        Value::s_ptr visit_Set(Expr::Set& expr) override;
        Value::s_ptr visit_This(Expr::This& expr) override;

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
        VariableResolveMapping _resolved_lookups;
        DiagnosticsTokenTracker _callstack;

        Value::s_ptr _evaluate(Expr& expr);
        Value::s_ptr _evaluate_optional(std::optional<Expr::u_ptr>& expr);
        void _interpret(Stmt& statement);

        Value::s_ptr _resolved_lookup(const Expr& access_expr, std::string identifier);

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
