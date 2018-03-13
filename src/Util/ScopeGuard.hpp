//
// Created by mafn on 3/13/18.
//

#ifndef HONK_SCOPEGUARD_HPP
#define HONK_SCOPEGUARD_HPP

namespace Honk
{
    namespace Util
    {
        struct I_Scopable
        {
            virtual ~I_Scopable() = default;

            virtual void scope_enter() = 0;
            virtual void scope_exit() = 0;
        };

        struct ScopeGuard
        {
            ScopeGuard(I_Scopable& target);
            ~ScopeGuard();

        private:
            I_Scopable& _target;
        };
    }
}

#endif //HONK_SCOPEGUARD_HPP
