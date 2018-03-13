//
// Created by mafn on 3/13/18.
//

#ifndef HONK_ASTCALLSTACK_HPP
#define HONK_ASTCALLSTACK_HPP

#include <stack>

#include "Lexer/Token.hpp"
#include "AST/Stmt.hpp"
#include "Util/Util.hpp"

namespace Honk
{
    // Should be used in combination with Util::ScopeGuard for exception safety
    struct DiagnosticsTokenTracker : Util::I_Scopable
    {
        const Token& get_last_token();

        void add_call(const Token*);
        void scope_enter() override;
        void scope_exit() override;

        std::stack<const Token*> _callstack;
    };
}


#endif //HONK_ASTCALLSTACK_HPP
