#pragma once

#include "Util/ScopeGuard.hpp"

namespace Honk
{
    enum struct FunctionType
    {
        NONE,
        FUNCTION,
        METHOD
    };

    struct CurrentFnContext : Util::I_Scopable<FunctionType>
    {
        CurrentFnContext(FunctionType& target);

        FunctionType& target;
        FunctionType old_value = FunctionType::NONE;

        void scope_enter(FunctionType context_type) override;
        void scope_exit();
    };
}