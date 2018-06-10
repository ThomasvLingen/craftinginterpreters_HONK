//
// Created by mafn on 3/13/18.
//

#ifndef HONK_SCOPEGUARD_HPP
#define HONK_SCOPEGUARD_HPP

#include <utility>

namespace Honk
{
    namespace Util
    {
        template<typename ...Args>
        struct I_Scopable
        {
            virtual ~I_Scopable() = default;

            virtual void scope_enter(Args ...args) = 0;
            virtual void scope_exit() = 0;
        };

        template<typename ...Args>
        struct ScopeGuard
        {
            ScopeGuard(I_Scopable<Args...>& target, Args... args)
                : _target(target)
            {
                this->_target.scope_enter(std::forward<Args>(args)...);
            }

            ~ScopeGuard()
            {
                this->_target.scope_exit();
            }

        private:
            I_Scopable<Args...>& _target;
        };
    }
}

#endif //HONK_SCOPEGUARD_HPP
