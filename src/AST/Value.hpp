//
// Created by mafn on 2/20/18.
//

#ifndef HONK_VALUE_HPP
#define HONK_VALUE_HPP

#include "Lexer/Token.hpp"

namespace Honk
{
    // This only exists to make interacting with Variant a bit more simple.
    // Also, the implementation could be swapped for a union + tagging as well.
    struct Value
    {
        TokenLiteral value;

        template <typename T>
        T* get()
        {
            return std::get_if<T>(&this->value);
        }

        template <typename T>
        T get_as() const
        {
            return std::get<T>(this->value);
        }

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
};


#endif //HONK_VALUE_HPP