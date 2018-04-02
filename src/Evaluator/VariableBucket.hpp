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
        using Bucket = std::unordered_map<std::string, Value>;

        VariableBucket() = default;
        VariableBucket(VariableBucket* enclosing);

        bool has_var_in_local(const std::string& identifier);
        void new_var(const std::string& identifier, Value initial_value);
        void new_var(NativeCallable native_fn);
        __maybe_nullptr __non_owning Value* get_var(const std::string& identifier);

    private:
        __maybe_nullptr __non_owning VariableBucket* _enclosing = nullptr;
        __maybe_nullptr Value* _try_get_from_enclosing(const std::string& identifier);
        Bucket _values;
    };

    // The same as a VariableBucket, but this supports multiple scopes.
    struct VariableBucket::Scoped : Util::I_Scopable
    {
        Scoped();

        void scope_enter() override;
        void scope_exit() override;
        VariableBucket& get_global_env();
        VariableBucket& get_current_env();

        size_t get_scope_depth();
    private:
        std::stack<VariableBucket> _scopes;
        VariableBucket& _global_scope;
    };
}


#endif //HONK_VARIABLEBUCKET_HPP
