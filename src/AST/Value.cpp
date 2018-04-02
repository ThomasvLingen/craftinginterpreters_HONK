//
// Created by mafn on 2/20/18.
//

#include "Value.hpp"

#include "Util/Util.hpp"
#include "AST/Callable.hpp"

namespace Honk
{
    std::string Value::to_str() const
    {
        if (this->is_a<std::string>()) {
            return "\"" + _to_string(this->value) + "\"";
        }

        if (this->is_a<bool>()) {
            return Util::bool_str(this->get_as<bool>());
        }

        return _to_string(this->value);
    }

    bool Value::is_null() const
    {
        return this->is_a<null_t>();
    }

    bool Value::is_integer() const
    {
        // For now this is just int64_t. Later this may be more int types
        return this->is_a<honk_int_t>();
    }

    bool operator==(const Value& a, const Value& b)
    {
        // This upkeeps the relational operator rules for std::variant, which are:
        // * Contained type not the same? false
        // * Valueless by exception? true
        // * Otherwise just a regular left == right
        return a.value == b.value;
    }

    Callable* Value::get_as_callable()
    {
        if (this->is_a<NativeCallable>()) {
            return this->get<NativeCallable>();
        }

        return nullptr;
    }
}
