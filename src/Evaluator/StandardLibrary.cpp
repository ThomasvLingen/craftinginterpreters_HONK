//
// Created by mafn on 4/2/18.
//

#include "StandardLibrary.hpp"

#include "Evaluator.hpp"

namespace Honk
{
    std::vector<NativeCallable> StandardLibrary::Native::_fns {
        {"honk_get_scope_depth", StandardLibrary::Native::get_scope_depth, 0},
    };

    Value StandardLibrary::Native::get_scope_depth(Evaluator& runtime, Arguments)
    {
        honk_int_t scope_depth = runtime._scopes.get_scope_depth();
        return Value{scope_depth};
    }

    void StandardLibrary::register_in(VariableBucket::Scoped& scopes)
    {
        VariableBucket& globals = scopes.get_global_env();

        for (NativeCallable& fn : StandardLibrary::Native::_fns) {
            globals.new_var(fn);
        }
    }
}