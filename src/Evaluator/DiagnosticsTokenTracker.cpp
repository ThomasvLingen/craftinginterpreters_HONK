//
// Created by mafn on 3/13/18.
//

#include "DiagnosticsTokenTracker.hpp"

namespace Honk
{
    const Token& DiagnosticsTokenTracker::get_last_token()
    {
        if (this->_callstack.size() > 0) {
            return *this->_callstack.top();
        }

        throw std::runtime_error { "Attempting to access call while callstack is empty" };
    }

    void DiagnosticsTokenTracker::scope_enter()
    {
        // Do nothing
    }

    void DiagnosticsTokenTracker::scope_exit()
    {
        this->_callstack.pop();
    }

    void DiagnosticsTokenTracker::add_call(const Token* call)
    {
        this->_callstack.push(call);
    }
}
