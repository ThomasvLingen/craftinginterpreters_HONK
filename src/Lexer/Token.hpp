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

namespace Honk
{
    enum struct TokenType : int
    {
        IDENTIFIER, INT, BOOL, STRING,

        PAREN_OPEN, PAREN_CLOSE, CURLY_OPEN, CURLY_CLOSE,
        COMMA, SEMICOLON,
        PLUS, MINUS, STAR, SLASH,

        NOT,

        EQUALS, NOT_EQUALS,
        GREATER, GREATER_EQ,
        LESS_THAN, LESS_THAN_EQ,

        FUN, RETURN,
        CONST, VAR, ASSIGNMENT, VAL_NULL,
        IF, ELSE, WHILE, FOR,
        AND, OR,

        END_OF_FILE
    };

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
        null_t
    >;

    // Helpers for stringifying
    std::string _to_string(TokenType type);
    std::string _to_string(TokenLiteral literal);

    // The order of members is used in aggregate initialisation.
    // Don't fuck with it.
    struct Token
    {
        TokenType type;
        std::string text;

        TokenLiteral value;

        // For error reporting
        uint32_t line;

        std::string to_str() const;
        bool has_value() const;
    };

    using TokenStream = std::vector<Token>;
}

#endif //HONK_TOKEN_HPP
