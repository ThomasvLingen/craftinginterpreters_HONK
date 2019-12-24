//
// Created by mafn on 4/15/18.
//

#ifndef HONK_FUNCTION_HPP
#define HONK_FUNCTION_HPP

#include "AST/Callable.hpp"
#include "AST/ExprFwd.hpp"

#include <type_traits>
#include <optional>
#include <memory>

namespace Honk
{
    struct ClassInstance;
    struct VariableBucket;

    struct Function : Callable
    {
        // A pointer is used to guarantee this being trivially move constructable.
        Function(std::optional<std::string> identifier, Expr::Fun* declaration, std::shared_ptr<VariableBucket> closure) noexcept;

        size_t n_args() const override;
        Value call(Evaluator& runtime, Arguments args) override;

        Function bind(std::shared_ptr<ClassInstance> instance);

        friend std::ostream& operator<<(std::ostream& os, const Function& obj);
        friend bool operator==(const Function& a, const Function& b);
    private:
        std::optional<std::string> _identifier;
        Expr::Fun* _declaration;
        std::shared_ptr<VariableBucket> _closure;

        void _define_args_in_bucket(VariableBucket& env, Arguments& args);
    };

    // static_assert(std::is_move_constructible<Function>::value);
    // static_assert(std::is_trivially_move_constructible<Function>::value);
}

#endif //HONK_FUNCTION_HPP

