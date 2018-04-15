//
// Created by mafn on 3/13/18.
//

#include "ScopeGuard.hpp"

#include <utility>

namespace Honk
{
    namespace Util
    {
        template<>
        ScopeGuard<>::ScopeGuard(I_Scopable<>& target)
            : _target(target)
        {
            this->_target.scope_enter();
        }

        template<typename ...Args>
        ScopeGuard<Args...>::ScopeGuard(I_Scopable<Args...>& target, Args&& ... args)
            : _target(target)
        {
            this->_target.scope_enter(std::forward<Args>(args)...);
        }

        template <>
        ScopeGuard<>::~ScopeGuard()
        {
            this->_target.scope_exit();
        }

        template<typename ...Args>
        ScopeGuard<Args...>::~ScopeGuard()
        {
            this->_target.scope_exit();
        }
    }
}