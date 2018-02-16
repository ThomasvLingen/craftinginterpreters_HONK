//
// Created by mafn on 11/9/17.
//

#include "Token.hpp"

#include <sstream>

namespace Honk
{
    bool Token::has_value() const
    {
        return this->type == (+TokenType::INT) ||
               this->type == (+TokenType::BOOL) ||
               this->type == (+TokenType::STRING);
    }

    std::string Token::to_str() const
    {
        std::stringstream str;

        str << "["   << this->type._to_string() << "]"
            << " '" << this->text              << "'";

        if (this->has_value()) {
            str << " = " << Token::literal_to_str(this->value);
        }

        str << "\n";

        return str.str();
    }

    std::string Token::literal_to_str(const TokenLiteral& literal)
    {
        std::stringstream str;

        auto value_variant_printer = [&str] (const auto& value) {
            str << value;
        };
        std::visit(value_variant_printer, literal);

        return str.str();
    }
}
