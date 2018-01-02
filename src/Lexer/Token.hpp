//
// Created by mafn on 11/9/17.
//

#ifndef HONK_TOKEN_HPP
#define HONK_TOKEN_HPP

#include <cstdint>
#include <string>

#include <variant>

namespace Honk
{
    enum struct TokenType
    {
        IDENTIFIER, INT, BOOL,

        PAREN_OPEN, PAREN_CLOSE, CURLY_OPEN, CURLY_CLOSE,
        COMMA, SEMICOLON,
        PLUS, MIN, STAR, SLASH,

        NOT,

        EQUALS, NOT_EQUALS,
        GREATER, GREATER_EQ,
        LESS_THAN, LESS_THAN_EQ,

        FUN, RETURN,
        CONST, VAR, ASSIGNMENT, VAL_NULL,
        IF, ELSE, WHILE, FOR,

        END_OF_FILE
    };

    struct Token
    {
        TokenType type;
        std::string text;

        std::variant<
            int32_t,
            std::string,
            bool
        > value;

        // For error reporting
        uint32_t line;
    };
}

#endif //HONK_TOKEN_HPP
