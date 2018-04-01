//
// Created by mafn on 4/1/18.
//

#include "TokenLiteral.hpp"

#include <sstream>

namespace Honk
{
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

    std::string _to_string(TokenLiteral literal)
    {
        std::stringstream str;

        auto value_variant_printer = [&str] (const auto& value) {
            str << value;
        };
        std::visit(value_variant_printer, literal);

        return str.str();
    }
}