//
// Created by mafn on 3/9/18.
//

#include "VariableBucket.hpp"

namespace Honk
{
    void VariableBucket::new_var(const std::string& identifier, Value initial_value)
    {
        // TODO: Maybe throw an error upon redefinition
        this->_values[identifier] = initial_value;
    }

    Value* VariableBucket::get_var(const std::string& identifier)
    {
        return Util::map_get_ptr(this->_values, identifier);
    }
}
