//
// Created by mafn on 2/20/18.
//

#ifndef HONK_VALUE_HPP
#define HONK_VALUE_HPP

#include <vector>
#include <memory>

#include "Lexer/TokenLiteral.hpp"

namespace Honk
{
    struct Callable;

    // This only exists to make interacting with Variant a bit more simple.
    // Also, the implementation could be swapped for a union + tagging as well.
    struct Value
    {
        using s_ptr = std::shared_ptr<Value>;
        TokenLiteral value;

        Value(TokenLiteral val);

        template <typename T>
        T* get_if()
        {
            return std::get_if<T>(&this->value);
        }

        template <typename T>
        T get_as() const
        {
            return std::get<T>(this->value);
        }

        Callable* get_as_callable();

        template<typename T>
        bool is_a() const
        {
            return std::holds_alternative<T>(this->value);
        }

        bool is_null() const;
        bool is_integer() const;
        std::string to_str() const;

        friend bool operator==(const Value& a, const Value& b);
    };

    using Arguments = std::vector<Value>;
};


#endif //HONK_VALUE_HPP
