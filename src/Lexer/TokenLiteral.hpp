//
// Created by mafn on 4/1/18.
//

#ifndef HONK_TOKENLITERAL_HPP
#define HONK_TOKENLITERAL_HPP

#include <string>
#include <cstdint>
#include <variant>

#include "AST/Callable.hpp"
#include "Evaluator/Function.hpp"
#include "Evaluator/Class.hpp"
#include "Evaluator/ClassInstance.hpp"

namespace Honk
{
    // This exists to represent a "null" value.
    struct null_t
    {
        friend std::ostream& operator<<(std::ostream& os, null_t);
        friend bool operator==(const null_t&, const null_t&);
    };
    static const null_t null {};

    using honk_int_t = int64_t;

    // TODO: think about passing by reference
    //       Implement using std::shared_ptr<ClassInstance> etc?
    using TokenLiteral = std::variant<
        std::string,
        honk_int_t,
        bool,
        null_t,
        NativeCallable,
        Function,
        Class,
        ClassInstance
    >;

    std::string _to_string(TokenLiteral literal);
}

#endif //HONK_TOKENLITERAL_HPP
