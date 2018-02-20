//
// Created by mafn on 2/20/18.
//

#include "Value.hpp"

namespace Honk
{
    std::string Value::to_str() const
    {
        return _to_string(this->value);
    }

    bool Value::is_null() const
    {
        return this->is_a<null_t>();
    }
}
