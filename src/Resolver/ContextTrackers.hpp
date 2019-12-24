#pragma once

#include "Util/ScopeGuard.hpp"

namespace Honk
{
    template <typename T>
    struct ContextTracker : Util::I_Scopable<T>
    {
        ContextTracker(T& target)
            : target(target) {}

        T& target;
        T old_value;

        void scope_enter(T new_value)
        {
            this->old_value = target;
            this->target = new_value;
        }

        void scope_exit()
        {
            this->target = old_value;
        }
    };

    enum struct FunctionType
    {
        NONE,
        FUNCTION,
        METHOD
    };

    enum struct ClassType
    {
        NONE,
        CLASS
    };

    using CurrentFnContext = ContextTracker<FunctionType>;
    using CurrentClassContext = ContextTracker<ClassType>;
}