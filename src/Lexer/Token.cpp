//
// Created by mafn on 11/9/17.
//

#include "Token.hpp"

#include <sstream>

namespace Honk
{
    bool Token::has_value() const
    {
        return this->type == TokenType::INT ||
               this->type == TokenType::BOOL ||
               this->type == TokenType::STRING;
    }

    std::string Token::to_str() const
    {
        std::stringstream str;

        str << "["   << _to_string(this->type) << "]"
            << " '" << this->text              << "'";

        if (this->has_value()) {
            str << " = " << _to_string(this->value);
        }

        return str.str();
    }

    std::string _to_string(TokenLiteral literal)
    {
        std::stringstream str;

        auto value_variant_printer = [&str] (const auto& value) {
            str << value;
        };
        std::visit(value_variant_printer, literal);

        return str.str();
    }

    std::string _to_string(TokenType type)
    {
        // There is still not a better way to do this.
        // May god have mercy on me
        #define shit_stringify(a) internal_shit_stringify(a)
        #define internal_shit_stringify(a) #a
        #define HONK_ENUM_STR(type) case TokenType::type : return shit_stringify(type);

        switch(type) {
            HONK_ENUM_STR(IDENTIFIER); HONK_ENUM_STR(INT); HONK_ENUM_STR(BOOL); HONK_ENUM_STR(STRING);

            HONK_ENUM_STR(PAREN_OPEN); HONK_ENUM_STR(PAREN_CLOSE); HONK_ENUM_STR(CURLY_OPEN); HONK_ENUM_STR(CURLY_CLOSE);
            HONK_ENUM_STR(COMMA); HONK_ENUM_STR(SEMICOLON);
            HONK_ENUM_STR(PLUS);HONK_ENUM_STR(MINUS);HONK_ENUM_STR(STAR);HONK_ENUM_STR(SLASH)

            HONK_ENUM_STR(NOT);

            HONK_ENUM_STR(EQUALS); HONK_ENUM_STR(NOT_EQUALS);
            HONK_ENUM_STR(GREATER); HONK_ENUM_STR(GREATER_EQ);
            HONK_ENUM_STR(LESS_THAN); HONK_ENUM_STR(LESS_THAN_EQ);

            HONK_ENUM_STR(FUN); HONK_ENUM_STR(RETURN);
            HONK_ENUM_STR(CONST); HONK_ENUM_STR(VAR); HONK_ENUM_STR(ASSIGNMENT); HONK_ENUM_STR(VAL_NULL);
            HONK_ENUM_STR(IF); HONK_ENUM_STR(ELSE); HONK_ENUM_STR(WHILE); HONK_ENUM_STR(FOR);
            HONK_ENUM_STR(AND); HONK_ENUM_STR(OR);

            HONK_ENUM_STR(END_OF_FILE);
            default:
                return "BIG FAT ERROR";
        }
    }

    std::ostream& operator<<(std::ostream& os, null_t)
    {
        os << "null";
        return os;
    }

    bool operator==(const null_t&, const null_t&)
    {
        // null is always equal to null
        return true;
    }
}
