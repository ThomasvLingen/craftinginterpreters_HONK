//
// Created by mafn on 3/9/18.
//

#ifndef HONK_VARIABLEBUCKET_HPP
#define HONK_VARIABLEBUCKET_HPP

#include <unordered_map>
#include <memory>
#include <stack>

#include "AST/Value.hpp"
#include "Util/Util.hpp"

namespace Honk
{
    // Important note, since std::optional references are impossible, non-owning possibly null pointers are used instead.
    struct VariableBucket
    {
        struct Scoped;
        using Bucket = std::unordered_map<std::string, Value::s_ptr>;
        using s_ptr = std::shared_ptr<VariableBucket>;

        VariableBucket() = default;
        VariableBucket(VariableBucket* enclosing);

        bool has_var_in_local(const std::string& identifier);
        void new_var(const std::string& identifier, Value initial_value);
        void new_var(NativeCallable native_fn);
        __maybe_nullptr __non_owning Value::s_ptr get_var(const std::string& identifier);

        __maybe_nullptr __non_owning VariableBucket* _enclosing = nullptr;

    private:
        __maybe_nullptr Value::s_ptr _try_get_from_enclosing(const std::string& identifier);
        Bucket _values;
    };

    // The same as a VariableBucket, but this supports multiple scopes.
    struct VariableBucket::Scoped : Util::I_Scopable<>, Util::I_Scopable<VariableBucket*>
    {
        Scoped();

        void scope_enter() override;
        void scope_enter(VariableBucket* enclosing) override;
        void scope_exit() override;

        VariableBucket& get_global_env();
        VariableBucket& get_current_env();
        VariableBucket::s_ptr claim_current_env();

        size_t get_scope_depth();
    private:
        std::stack<VariableBucket::s_ptr> _scopes;
        VariableBucket& _global_scope;
    };
}


#endif //HONK_VARIABLEBUCKET_HPP
