//
// Created by mafn on 4/1/18.
//

#ifndef HONK_CALLABLE_HPP
#define HONK_CALLABLE_HPP

#include <vector>
#include <string>

namespace Honk
{
    struct Evaluator;
    struct Value;
    using Arguments = std::vector<Value>;

    struct Callable
    {
        virtual Value call(Evaluator& runtime, Arguments args) = 0;
        virtual size_t n_args() const = 0;

        using fn_sig = Value (*) (Evaluator& runtime, Arguments args);
    };

    struct NativeCallable : Callable
    {
        NativeCallable(std::string identifier, Callable::fn_sig cpp_function, size_t n_args);

        Value call(Evaluator& runtime, Arguments args) override;
        size_t n_args() const override;

        std::string identifier;

        friend std::ostream& operator<<(std::ostream& os, const NativeCallable& obj);
        friend bool operator==(const NativeCallable& a, const NativeCallable& b);
    private:
        Callable::fn_sig _cpp_function;
        size_t _n_args;
    };
}


#endif //HONK_CALLABLE_HPP
