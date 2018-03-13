//
// Created by mafn on 3/13/18.
//

#include "ScopeGuard.hpp"

namespace Honk
{
    namespace Util
    {
        ScopeGuard::ScopeGuard(I_Scopable& target)
            : _target(target)
        {
            this->_target.scope_enter();
        }

        ScopeGuard::~ScopeGuard()
        {
            this->_target.scope_exit();
        }
    }
}