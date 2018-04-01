//
// Created by mafn on 4/1/18.
//

#include "Callable.hpp"

#include "Evaluator/Evaluator.hpp"
#include "AST/Value.hpp"

namespace Honk
{
    NativeCallable::NativeCallable(std::string identifier, Callable::fn_sig cpp_function, size_t n_args)
        : identifier(identifier)
        , _cpp_function(cpp_function)
        , _n_args(n_args)
    {
    }

    Value NativeCallable::call(Evaluator& runtime, Arguments args)
    {
        return this->_cpp_function(runtime, args);
    }

    size_t NativeCallable::n_args() const
    {
        return this->_n_args;
    }

    std::ostream& operator<<(std::ostream& os, const NativeCallable& obj)
    {
        os << obj.identifier;
        os << "(" << obj.n_args() << " args)";
        return os;
    }

    bool operator==(const NativeCallable& a, const NativeCallable& b)
    {
        return a.identifier == b.identifier;
    }
}
