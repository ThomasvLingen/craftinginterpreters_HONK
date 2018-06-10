//
// Created by mafn on 4/15/18.
//

#ifndef HONK_FUNCTION_HPP
#define HONK_FUNCTION_HPP

#include "AST/Callable.hpp"
#include "AST/StmtFwd.hpp"

#include <type_traits>

namespace Honk
{
    struct VariableBucket;

    struct Function : Callable
    {
        // A pointer is used to guarantee this being trivially move constructable.
        Function(Stmt::FunDeclaration* declaration, std::shared_ptr<VariableBucket> closure) noexcept;

        size_t n_args() const override;
        Value call(Evaluator& runtime, Arguments args) override;

        friend std::ostream& operator<<(std::ostream& os, const Function& obj);
        friend bool operator==(const Function& a, const Function& b);
    private:
        Stmt::FunDeclaration* _declaration;
        std::shared_ptr<VariableBucket> _closure;

        void _define_args_in_bucket(VariableBucket& env, Arguments& args);
    };

    // static_assert(std::is_move_constructible<Function>::value);
    // static_assert(std::is_trivially_move_constructible<Function>::value);
}

#endif //HONK_FUNCTION_HPP

