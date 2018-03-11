//
// Created by mafn on 3/9/18.
//

#ifndef HONK_VARIABLEBUCKET_HPP
#define HONK_VARIABLEBUCKET_HPP

#include <unordered_map>
#include <memory>
#include <stack>

#include "AST/Value.hpp"
#include "Util.hpp"

namespace Honk
{
    // Important note, since std::optional references are impossible, non-owning possibly null pointers are used instead.
    struct VariableBucket
    {
        struct Scoped;
        using Bucket = std::unordered_map<std::string, Value>;

        VariableBucket() = default;
        VariableBucket(VariableBucket* enclosing);

        void new_var(const std::string& identifier, Value initial_value);
        __maybe_nullptr __non_owning Value* get_var(const std::string& identifier);

    private:
        __maybe_nullptr __non_owning VariableBucket* _enclosing = nullptr;
        Bucket _values;
    };

    // The same as a VariableBucket, but this supports multiple scopes.
    struct VariableBucket::Scoped
    {
        struct Guard;

        Scoped();

        void enter_scope();
        void exit_scope();
        VariableBucket& get_current_env();

    private:
        std::stack<VariableBucket> _scopes;
    };

    // Lock guard for use in entering and exiting scopes
    struct VariableBucket::Scoped::Guard
    {
        Guard(VariableBucket::Scoped& target);
        ~Guard();

    private:
        VariableBucket::Scoped& _target;
    };
}


#endif //HONK_VARIABLEBUCKET_HPP
