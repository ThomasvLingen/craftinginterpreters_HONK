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
        this->_values[identifier] = std::make_shared<Value>(initial_value);
    }

    void VariableBucket::new_var(NativeCallable native_fn)
    {
        this->new_var(native_fn.identifier, Value {native_fn});
    }

    Value::s_ptr VariableBucket::get_var(const std::string& identifier)
    {
        Value::s_ptr local_var = Util::map_get_s_ptr(this->_values, identifier);
        if (local_var) {
            return local_var;
        }

        return this->_try_get_from_enclosing(identifier);
    }

    bool VariableBucket::has_var_in_local(const std::string& identifier)
    {
        return Util::map_get_ptr(this->_values, identifier) != nullptr;
    }

    Value::s_ptr VariableBucket::_try_get_from_enclosing(const std::string& identifier)
    {
        if (!this->_enclosing) {
            return nullptr;
        }

        return this->_enclosing->get_var(identifier);
    }

    VariableBucket::Scoped::Scoped()
        : _scopes({ std::make_shared<VariableBucket>() }) // global scope
        , _global_scope(*_scopes.top())
    {
    }

    VariableBucket& VariableBucket::Scoped::get_current_env()
    {
        return *this->_scopes.top();
    }

    void VariableBucket::Scoped::scope_enter()
    {
        VariableBucket& enclosing = this->get_current_env();

        this->_scopes.push(std::make_shared<VariableBucket>(&enclosing));
    }

    void VariableBucket::Scoped::scope_enter(VariableBucket* enclosing)
    {
        this->_scopes.push(std::make_shared<VariableBucket>(enclosing));
    }

    void VariableBucket::Scoped::scope_exit()
    {
        this->_scopes.pop();
    }

    size_t VariableBucket::Scoped::get_scope_depth()
    {
        return this->_scopes.size();
    }

    VariableBucket& VariableBucket::Scoped::get_global_env()
    {
        return this->_global_scope;
    }

    VariableBucket::s_ptr VariableBucket::Scoped::claim_current_env()
    {
        return this->_scopes.top();
    }
}
