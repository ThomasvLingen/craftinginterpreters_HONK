//
// Created by mafn on 4/1/18.
//

#ifndef HONK_TOKENLITERAL_HPP
#define HONK_TOKENLITERAL_HPP

#include <string>
#include <cstdint>
#include <variant>

#include "AST/Callable.hpp"

namespace Honk
{
    // This exists to represent a "null" value.
    struct null_t
    {
        friend std::ostream& operator<<(std::ostream& os, null_t);
        friend bool operator==(const null_t&, const null_t&);
    };
    static const null_t null {};

    using TokenLiteral = std::variant<
        std::string,
        int32_t,
        bool,
        null_t,
        NativeCallable
    >;

    std::string _to_string(TokenLiteral literal);
}

#endif //HONK_TOKENLITERAL_HPP
