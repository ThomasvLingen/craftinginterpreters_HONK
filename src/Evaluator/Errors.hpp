//
// Created by mafn on 12/14/18.
//

#ifndef HONK_ERRORS_HPP
#define HONK_ERRORS_HPP

#include <stdexcept>

#include "Lexer/Token.hpp"

namespace Honk
{
    struct EvaluateError : std::runtime_error
    {
        EvaluateError(const char* msg, const Token* error_token)
            : std::runtime_error(msg)
            , error_token(error_token)
        {
        }

        const Token* error_token = nullptr;
    };
}

#endif //HONK_ERRORS_HPP
