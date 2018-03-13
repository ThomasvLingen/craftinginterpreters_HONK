//
// Created by mafn on 3/9/18.
//

#include "VariableBucket.hpp"

namespace Honk
{
    VariableBucket::VariableBucket(VariableBucket* enclosing)
        : _enclosing(enclosing)
    {
    }

    void VariableBucket::new_var(const std::string& identifier, Value initial_value)
    {
        this->_values[identifier] = initial_value;
    }

    Value* VariableBucket::get_var(const std::string& identifier)
    {
        Value* local_var = Util::map_get_ptr(this->_values, identifier);
        if (local_var) {
            return local_var;
        }

        return this->_enclosing->get_var(identifier);
    }

    bool VariableBucket::has_var_in_local(const std::string& identifier)
    {
        return Util::map_get_ptr(this->_values, identifier) != nullptr;
    }

    VariableBucket::Scoped::Scoped()
    {
        // Initial (global) scope
        this->_scopes.push(VariableBucket {});
    }

    VariableBucket& VariableBucket::Scoped::get_current_env()
    {
        return this->_scopes.top();
    }

    void VariableBucket::Scoped::scope_enter()
    {
        VariableBucket& enclosing = this->get_current_env();

        this->_scopes.push(VariableBucket(&enclosing));
    }

    void VariableBucket::Scoped::scope_exit()
    {
        this->_scopes.pop();
    }
}
