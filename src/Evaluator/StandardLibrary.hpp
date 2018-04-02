//
// Created by mafn on 4/2/18.
//

#ifndef HONK_STANDARDLIBRARY_HPP
#define HONK_STANDARDLIBRARY_HPP

#include <vector>

#include "AST/Value.hpp"
#include "AST/Callable.hpp"
#include "VariableBucket.hpp"

namespace Honk
{
    struct Evaluator;

    struct StandardLibrary
    {
        static void register_in(VariableBucket::Scoped& scopes);

        struct Native
        {
            static std::vector<NativeCallable> _fns;

        private:
            static Value get_scope_depth(Evaluator& runtime, Arguments);
        };
    };
}


#endif //HONK_STANDARDLIBRARY_HPP
