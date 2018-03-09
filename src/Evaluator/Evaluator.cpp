//
// Created by mafn on 2/25/18.
//

#include "Evaluator.hpp"

#include <iostream>

#include "Interpreter.hpp"

namespace Honk
{
    Evaluator::Evaluator(const Interpreter& parent)
        : _parent(parent)
    {
    }

    void Evaluator::interpret(Stmt::stream& code)
    {
        try {
            for (Stmt::u_ptr& statement : code) {
                this->_interpret(*statement);
            }
        } catch (std::runtime_error& e) {
            // TODO: make sure the line number is correct here
            this->_parent.report_error(0, e.what());
        }
    }

    Value Evaluator::evaluate(Expr& expression)
    {
        try {
            return this->_evaluate(expression);
        } catch (std::runtime_error& e) {
            // TODO: make sure the line number is correct here
            this->_parent.report_error(0, e.what());
        }
    }

    Value Evaluator::visitLiteral(Expr::Literal& expr)
    {
        return expr.value;
    }

    Value Evaluator::visitGrouped(Expr::Grouped& expr)
    {
        return this->_evaluate(*expr.expression);
    }

    Value Evaluator::_evaluate(Expr& expr)
    {
        return expr.accept(*this);
    }

    void Evaluator::_interpret(Stmt& statement)
    {
        statement.accept(*this);
    }

    Value Evaluator::visitUnary(Expr::Unary& expr)
    {
        Value right = this->_evaluate(*expr.right);

        if (expr.op_type() == TokenType::MINUS) {
            if (int32_t* val = right.get<int32_t>()) {
                return Value {-*val};
            }

            throw std::runtime_error {"Unary expression cannot be evaluated: minus used on something other than an integer"};
        }

        if (expr.op_type() == TokenType::NOT) {
            return Value {!this->_is_truthy(right)};
        }

        throw std::runtime_error {"Unary expression cannot be evaluated: invalid operator"};
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
            return val.get_as<int32_t>() != 0;
        }

        return val.get_as<bool>();
    }

    bool Evaluator::_is_equal(const Value& a, const Value& b)
    {
        // Small layer of abstraction, in case this needs to be changed.
        return a == b;
    }

    Value Evaluator::visitBinary(Expr::Binary& expr)
    {
        // Binary expressions are evaluated left to right
        Value left = this->_evaluate(*expr.left);
        Value right = this->_evaluate(*expr.right);

        // I felt that BinaryExpressions were too complex to just stuff it all in one giant function
        // Therefore I made a BinaryExpressionVisitor, which selects the right sub-member from the this class
        return expr.accept(*this, left, right);
    }

    Value Evaluator::visit_minus(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<int32_t>(left, right, "Substraction expression cannot be evaluated: operands are not integers");

        return Value {left_val - right_val};
    }

    Value Evaluator::visit_slash(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<int32_t>(left, right, "Division expression cannot be evaluated: operands are not integers");

        if (right_val == 0) {
            throw std::runtime_error {"Division expression cannot be evaluated: divide by 0"};
        }

        return Value {left_val / right_val};
    }

    Value Evaluator::visit_star(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<int32_t>(left, right, "Multiplication expression cannot be evaluated: operands are not integers");

        return Value {left_val * right_val};
    }

    Value Evaluator::visit_plus(const Value& left, const Value& right)
    {
        // TODO: Might be able to refactor this into a single std::visit call on top of the variant inside of Value...
        if (this->_values_are<int32_t>(left, right)) {
            auto [left_val, right_val] = this->_get_as<int32_t>(left, right);
            return Value {left_val + right_val};
        }

        if (this->_values_are<std::string>(left, right)) {
            auto [left_val, right_val] = this->_get_as<std::string>(left, right);
            return Value {left_val + right_val};
        }

        throw std::runtime_error{"Plus expression cannot be evaluated: operands aren't either strings or integers"};
    }

    Value Evaluator::visit_greater(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<int32_t>(left, right, "Greater Than expression cannot be evaluated: operands are not integers");

        return Value { left_val > right_val };
    }

    Value Evaluator::visit_greater_eq(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<int32_t>(left, right, "Greater Than Or Equal To expression cannot be evaluated: operands are not integers");

        return Value {left_val >= right_val};
    }

    Value Evaluator::visit_less_than(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<int32_t>(left, right, "Less Than expression cannot be evaluated: operands are not integers");

        return Value {left_val < right_val};
    }

    Value Evaluator::visit_less_than_eq(const Value& left, const Value& right)
    {
        auto [left_val, right_val] = this->_try_get_as<int32_t>(left, right, "Less Than Or Equal To expression cannot be evaluated: operands are not integers");

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

    Value Evaluator::visitVarAccess(Expr::VarAccess& expr)
    {
        Value* accessed_value = this->_env.get_var(expr.get_identifier());

        if (!accessed_value) {
            throw std::runtime_error {"Accessing an undefined variable: " + expr.get_identifier()};
        }

        return *accessed_value;
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
            throw std::runtime_error{throw_msg};
        }

        return this->_get_as<T>(left, right);
    }

    void Evaluator::_throw_if_not_integer(const Value& left, const Value& right, const char* throw_msg)
    {
        if (!this->_values_are<int32_t>(left, right)) {
            throw std::runtime_error {throw_msg};
        }
    }

    void Evaluator::visit_Expression(Stmt::Expression& stmt)
    {
        // Nothing is done with this
        // TODO: might want to print this or emit a warning?
        this->_evaluate(*stmt.expression);
    }

    void Evaluator::visit_Print(Stmt::Print& stmt)
    {
        Value expression_result = this->_evaluate(*stmt.expression);
        std::cout << expression_result.to_str() << std::endl;
    }

    void Evaluator::visit_VarDeclaration(Stmt::VarDeclaration& stmt)
    {
        Value initial_value { null };

        if (stmt.initializer) {
            initial_value = this->_evaluate(**stmt.initializer);
        }

        this->_env.new_var(stmt.get_identifier(), initial_value);
    }
}