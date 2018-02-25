//
// Created by mafn on 2/20/18.
//

#include "Value.hpp"

namespace Honk
{
    std::string Value::to_str() const
    {
        // TODO: Some of this is pretty crappy:
        // * booleans print "1" or "0"
        // * strings are printed without quotes
        return _to_string(this->value);
    }

    bool Value::is_null() const
    {
        return this->is_a<null_t>();
    }

    bool Value::is_integer() const
    {
        // For now this is just int32_t. Later this may be more int types
        return this->is_a<int32_t>();
    }

    bool operator==(const Value& a, const Value& b)
    {
        // This upkeeps the relational operator rules for std::variant, which are:
        // * Contained type not the same? false
        // * Valueless by exception? true
        // * Otherwise just a regular left == right
        return a.value == b.value;
    }
}
