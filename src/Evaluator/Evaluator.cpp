//
// Created by mafn on 2/25/18.
//

#include "Evaluator.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "Interpreter.hpp"
#include "StandardLibrary.hpp"
#include "Function.hpp"
#include "Class.hpp"
#include "ClassInstance.hpp"

namespace Honk
{
    Evaluator::Evaluator(const Interpreter& parent)
        : _parent(parent)
    {
        StandardLibrary::register_in(this->scopes);
    }

    Return::Return(Value returned_value)
        : returned_value(returned_value)
    {
    }

    void Evaluator::interpret(Stmt::stream& code)
    {
        try {
            for (Stmt::u_ptr& statement : code) {
                this->_interpret(*statement);
            }
        } catch (EvaluateError& e) {
            this->_parent.report_error(e.error_token->line, e.what());
        }
    }

    Value Evaluator::evaluate(Expr& expression)
    {
        try {
            return *this->_evaluate(expression);
        } catch (EvaluateError& e) {
            this->_parent.report_error(e.error_token->line, e.what());

            return { null };
        }
    }

    Value::s_ptr Evaluator::visit_Literal(Expr::Literal& expr)
    {
        return std::make_shared<Value>(expr.value);
    }

    Value::s_ptr Evaluator::visit_Grouped(Expr::Grouped& expr)
    {
        return this->_evaluate(*expr.expression);
    }

    Value::s_ptr Evaluator::_evaluate(Expr& expr)
    {
        this->_callstack.add_call(&expr.diagnostics_token);

        Util::ScopeGuard guard(this->_callstack);
        return expr.accept(*this);
    }

    void Evaluator::_interpret(Stmt& statement)
    {
        this->_callstack.add_call(&statement.diagnostics_token);

        Util::ScopeGuard guard(this->_callstack);
        statement.accept(*this);
    }

    Value::s_ptr Evaluator::visit_Unary(Expr::Unary& expr)
    {
        Value right = *this->_evaluate(*expr.right);

        if (expr.op_type() == TokenType::MINUS) {
            if (honk_int_t* val = right.get_if<honk_int_t>()) {
                return std::make_shared<Value>(-*val);
            }

            throw this->_error("Unary expression cannot be evaluated: minus used on something other than an integer");
        }

        if (expr.op_type() == TokenType::NOT) {
            return std::make_shared<Value>(!this->_is_truthy(right));
        }

        throw this->_error("Unary expression cannot be evaluated: invalid operator");
    }

    bool Evaluator::_is_truthy(const Value& val)
    {
        if (val.is_null()) {
            return false;
        }

        if (val.is_a<std::string>()) {
            return !val.get_as<std::string>().empty();
        }

        if (val.is_integer()) {
            return val.get_as<honk_int_t>() != 0;
        }

        return val.get_as<bool>();
    }

    bool Evaluator::_is_equal(const Value& a, const Value& b)
    {
        // Small layer of abstraction, in case this needs to be changed.
        return a == b;
    }

    Value::s_ptr Evaluator::visit_Binary(Expr::Binary& expr)
    {
        // Binary expressions are evaluated left to right
        Value left = *this->_evaluate(*expr.left);
        Value right = *this->_evaluate(*expr.right);

        // I felt that BinaryExpressions were too complex to just stuff it all in one giant function
        // Therefore I made a BinaryExpressionVisitor, which selects the right sub-member from the this class
        return std::make_shared<Value> (
            expr.accept(*this, left, right)
        );
    }

    Value Evaluator::visit_minus(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<honk_int_t>(left, right, "Substraction expression cannot be evaluated: operands are not integers");

        return Value {left_val - right_val};
    }

    Value Evaluator::visit_slash(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<honk_int_t>(left, right, "Division expression cannot be evaluated: operands are not integers");

        if (right_val == 0) {
            throw this->_error("Division expression cannot be evaluated: divide by 0");
        }

        return Value {left_val / right_val};
    }

    Value Evaluator::visit_star(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<honk_int_t>(left, right, "Multiplication expression cannot be evaluated: operands are not integers");

        return Value {left_val * right_val};
    }

    Value Evaluator::visit_plus(const Value& left, const Value& right)
    {
        // TODO: Might be able to refactor this into a single std::visit call on top of the variant inside of Value...
        if (this->_values_are<honk_int_t>(left, right)) {
            auto [left_val, right_val] = this->_get_as<honk_int_t>(left, right);
            return Value {left_val + right_val};
        }

        if (this->_values_are<std::string>(left, right)) {
            auto [left_val, right_val] = this->_get_as<std::string>(left, right);
            return Value {left_val + right_val};
        }

        throw this->_error("Plus expression cannot be evaluated: operands aren't either strings or integers");
    }

    Value Evaluator::visit_greater(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<honk_int_t>(left, right, "Greater Than expression cannot be evaluated: operands are not integers");

        return Value { left_val > right_val };
    }

    Value Evaluator::visit_greater_eq(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<honk_int_t>(left, right, "Greater Than Or Equal To expression cannot be evaluated: operands are not integers");

        return Value {left_val >= right_val};
    }

    Value Evaluator::visit_less_than(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<honk_int_t>(left, right, "Less Than expression cannot be evaluated: operands are not integers");

        return Value {left_val < right_val};
    }

    Value Evaluator::visit_less_than_eq(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<honk_int_t>(left, right, "Less Than Or Equal To expression cannot be evaluated: operands are not integers");

        return Value {left_val <= right_val};
    }

    Value Evaluator::visit_equals(const Value& left, const Value& right)
    {
        return Value {this->_is_equal(left, right)};
    }

    Value Evaluator::visit_not_equals(const Value& left, const Value& right)
    {
        return Value {!this->_is_equal(left, right)};
    }

    Value::s_ptr Evaluator::visit_VarAccess(Expr::VarAccess& expr)
    {
        Value::s_ptr accessed_value = this->_resolved_lookup(expr, expr.get_identifier());

        if (!accessed_value) {
            throw this->_error("Accessing an undefined variable: " + expr.get_identifier());
        }

        return accessed_value;
    }

    Value::s_ptr Evaluator::visit_VarAssign(Expr::VarAssign& expr)
    {
        Value::s_ptr assigned_value = this->_resolved_lookup(expr, expr.get_identifier());

        if (!assigned_value) {
            throw this->_error("Trying to assign to an undefined variable: " + expr.get_identifier());
        }

        return std::make_shared<Value>(
            *assigned_value = *this->_evaluate(*expr.new_value)
        );
    }

    Value::s_ptr Evaluator::visit_LogicalOr(Expr::LogicalOr& expr)
    {
        Value::s_ptr left = this->_evaluate(*expr.left);

        if (this->_is_truthy(*left)) {
            return left;
        }

        return this->_evaluate(*expr.right);
    }

    Value::s_ptr Evaluator::visit_LogicalAnd(Expr::LogicalAnd& expr)
    {
        Value::s_ptr left = this->_evaluate(*expr.left);

        if (!this->_is_truthy(*left)) {
            return left;
        }

        return this->_evaluate(*expr.right);
    }

    Value::s_ptr Evaluator::visit_Call(Expr::Call& expr)
    {
        Value::s_ptr callee = this->_evaluate(*expr.callee);

        Arguments args = Util::map<Arguments>(expr.args,
            [this] (Expr::u_ptr& arg_expr) {
                return *this->_evaluate(*arg_expr);
            });

        if (Callable* function = callee->get_as_callable()) {
            if (args.size() != function->n_args()) {
                throw this->_invalid_call_error(function->n_args(), args.size());
            }

            return std::make_shared<Value>(function->call(*this, args));
        } else {
            throw this->_error("Attempting to call a non-callable value");
        }
    }

    Value::s_ptr Evaluator::visit_Fun(Expr::Fun& expr)
    {
        return std::make_shared<Value> (
            Function { std::nullopt, &expr, this->claim_env()}
        );
    }

    Value::s_ptr Evaluator::visit_Get(Expr::Get& expr)
    {
        Value::s_ptr target = this->_evaluate(*expr.get_target);
        if (!target->is_a<ClassInstance::s_ptr>()) {
            throw this->_error("Attempting to get a field from something other than a class instance");
        }

        std::string identifier = Token::get_text(expr.identifier_tok);

        // NOTE: The second arrow really makes a big difference here   v
        Value::s_ptr gotten_value = target->get<ClassInstance::s_ptr>()->get_field(identifier);
        if (!gotten_value) {
            throw this->_error("Attempting to access an undefined field");
        }
        return gotten_value;
    }

    Value::s_ptr Evaluator::visit_Set(Expr::Set& expr)
    {
        Value::s_ptr target = this->_evaluate(*expr.set_target);
        if (!target->is_a<ClassInstance::s_ptr>()) {
            throw this->_error("Attempting to set a field from something other than a class instance");
        }

        Value::s_ptr set_value = this->_evaluate(*expr.new_value);
        bool ok = target->get<ClassInstance::s_ptr>()->set_field(expr.identifier_tok.text, set_value);
        if (!ok) {
            throw this->_error("Attempting to set a non-declared field");
        }
        return set_value;
    }

    Value::s_ptr Evaluator::visit_This(Expr::This& expr)
    {
        Value::s_ptr resolved_this = this->_resolved_lookup(expr, expr.this_tok.text);

        if (!resolved_this) {
            throw this->_error("Invalid lookup of this");
        }

        return resolved_this;
    }

    template<typename T>
    std::pair<T, T> Evaluator::_get_as(const Value& left, const Value& right)
    {
        return std::make_pair<T, T>(left.get_as<T>(), right.get_as<T>());
    }

    template<typename T>
    bool Evaluator::_values_are(const Value& left, const Value& right)
    {
        return left.is_a<T>() && right.is_a<T>();
    }

    template<typename T>
    std::pair<T, T> Evaluator::_try_get_as(const Value& left, const Value& right, const char* throw_msg)
    {
        if (!this->_values_are<T>(left, right)) {
            throw this->_error(throw_msg);
        }

        return this->_get_as<T>(left, right);
    }

    void Evaluator::visit_Expression(Stmt::Expression& stmt)
    {
        // Nothing is done with this
        this->_evaluate(*stmt.expression);
    }

    void Evaluator::visit_Block(Stmt::Block& stmt)
    {
        // A scope guard is used to guarantee exception safety
        Util::ScopeGuard<> guard(this->scopes);
        this->execute_block(stmt);
    }

    void Evaluator::execute_block(Stmt::Block& block)
    {
        for (Stmt::u_ptr& block_stmt : block.statements) {
            this->_interpret(*block_stmt);
        }
    }

    void Evaluator::visit_If(Stmt::If& stmt)
    {
        if (this->_is_truthy(*stmt.condition)) {
            this->_interpret(*stmt.true_branch);
        } else if (stmt.false_branch) {
            this->_interpret(**stmt.false_branch);
        }
    }

    void Evaluator::visit_VarDeclaration(Stmt::VarDeclaration& stmt)
    {
        std::string identifier = stmt.get_identifier();
        if (this->env().has_var_in_local(identifier)) {
            throw this->_error("Redeclaring variable in same scope: " + identifier);
        }

        Value initial_value = *this->_evaluate_optional(stmt.initializer);
        this->env().new_var(identifier, initial_value);
    }

    void Evaluator::visit_While(Stmt::While& stmt)
    {
        while (this->_is_truthy(*stmt.condition)) {
            this->_interpret(*stmt.body);
        }
    }

    void Evaluator::visit_For(Stmt::For& stmt)
    {
        // Make a new scope for the for loop's clauses
        Util::ScopeGuard<> guard(this->scopes);

        // Execute the initializer
        if (stmt.initializer) {
            this->_interpret(**stmt.initializer);
        }

        // Main for body loop
        while (this->_is_truthy(*stmt.condition)) {
            this->_interpret(*stmt.body);
            if (stmt.increment) {
                this->_evaluate(**stmt.increment);
            }
        }
    }

    void Evaluator::visit_FunDeclaration(Stmt::FunDeclaration& stmt)
    {
        Function fun(stmt.identifier.text, &stmt.get_fun(), this->claim_env());
        this->env().new_var(stmt.get_identifier(), Value {fun});
    }

    void Evaluator::visit_Return(Stmt::Return& stmt)
    {
        throw Return(*this->_evaluate_optional(stmt.return_value));
    }

    void Evaluator::visit_Class(Stmt::Class& stmt)
    {
        std::vector<std::string> fields = Util::map<std::vector<std::string>> (stmt.fields,
            [] (Stmt::VarDeclaration::u_ptr& field_decl) {
                return field_decl->get_identifier();
            });

        Class::MethodMap methods = Util::map_to_stlmap<Class::MethodMap> (stmt.methods,
            [this] (const Stmt::FunDeclaration::u_ptr& method_decl) {
                std::string name = method_decl->get_identifier();

                return std::make_pair(
                    name, Function(name, &method_decl->get_fun(), this->claim_env())
                );
            });

        Class cls(stmt.name.text, fields, methods);
        this->env().new_var(stmt.name.text, Value {cls});
    }

    VariableBucket& Evaluator::env()
    {
        return this->scopes.get_current_env();
    }

    VariableBucket::s_ptr Evaluator::claim_env()
    {
        return this->scopes.claim_current_env();
    }

    bool Evaluator::_is_truthy(Expr& expr)
    {
        return this->_is_truthy(*this->_evaluate(expr));
    }

    Value::s_ptr Evaluator::_evaluate_optional(std::optional<Expr::u_ptr>& expr)
    {
        if (!expr) {
            return std::make_shared<Value>(null);
        }

        return this->_evaluate(**expr);
    }

    EvaluateError Evaluator::_error(const std::string& msg)
    {
        return EvaluateError(msg.c_str(), &this->_callstack.get_last_token());
    }

    EvaluateError Evaluator::_invalid_call_error(size_t expected_n_args, size_t actual)
    {
        std::stringstream error_msg;
        error_msg << "Invalid call, expected " << expected_n_args
                  << " arguments, got " << actual << ".";

        return this->_error(error_msg.str());
    }

    void Evaluator::add_resolves(const VariableResolveMapping& to_add)
    {
        this->_resolved_lookups.insert(to_add.cbegin(), to_add.cend());
    }

    Value::s_ptr Evaluator::_resolved_lookup(const Expr& access_expr, std::string identifier)
    {
        VariableBucket& target_env = this->get_target_env(access_expr);

        return target_env.get_var(identifier);
    }

    VariableBucket& Evaluator::env(size_t indirections)
    {
        VariableBucket* target_env = &this->env();

        while (indirections-- > 0) {
            target_env = target_env->_enclosing;
        }

        return *target_env;
    }

    VariableBucket& Evaluator::get_target_env(const Expr& access_expr)
    {
        if (!Util::contains(this->_resolved_lookups, &access_expr)){
            return this->globals;
        }

        const ResolvedLookup& lookup = this->_resolved_lookups[&access_expr];
        return this->env(lookup.indirections);
    }
}