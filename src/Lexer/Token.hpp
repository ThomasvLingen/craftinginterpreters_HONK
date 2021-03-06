//
// Created by mafn on 11/9/17.
//

#ifndef HONK_TOKEN_HPP
#define HONK_TOKEN_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <ostream>
#include <optional>

#include "TokenLiteral.hpp"

namespace Honk
{
    enum struct TokenType : int
    {
        IDENTIFIER, INT, BOOL, STRING,

        PAREN_OPEN, PAREN_CLOSE, CURLY_OPEN, CURLY_CLOSE,
        COMMA, SEMICOLON, DOT,
        PLUS, MINUS, STAR, SLASH,

        NOT,

        EQUALS, NOT_EQUALS,
        GREATER, GREATER_EQ,
        LESS_THAN, LESS_THAN_EQ,

        CLASS, THIS,
        FUN, RETURN,
        CONST, VAR, ASSIGNMENT, VAL_NULL,
        IF, ELSE, WHILE, FOR,
        AND, OR,

        END_OF_FILE
    };

    // Helpers for stringifying
    std::string _to_string(TokenType type);

    // The order of members is used in aggregate initialisation.
    // Don't fuck with it.
    struct Token
    {
        using opt = std::optional<Token>;

        TokenType type;
        std::string text;

        TokenLiteral value;

        // For error reporting
        uint32_t line;

        std::string to_str() const;
        bool has_value() const;

        static Token null;

        static std::string get_text(const Token& token);
    };

    using TokenStream = std::vector<Token>;
}

#endif //HONK_TOKEN_HPP
