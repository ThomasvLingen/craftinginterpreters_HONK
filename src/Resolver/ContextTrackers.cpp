#include "ContextTrackers.hpp"

namespace Honk
{
    CurrentFnContext::CurrentFnContext(FunctionType& target)
        : target(target) {}

    void CurrentFnContext::scope_enter(FunctionType context_type)
    {
        this->old_value = target;
        this->target = context_type;
    }

    void CurrentFnContext::scope_exit()
    {
        this->target = old_value;
    }
}
