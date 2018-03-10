//
// Created by mafn on 3/9/18.
//

#ifndef HONK_VARIABLEBUCKET_HPP
#define HONK_VARIABLEBUCKET_HPP

#include <unordered_map>

#include "AST/Value.hpp"
#include "Util.hpp"

namespace Honk
{
    // Important note, since std::optional references are impossible, non-owning possibly null pointers are used instead.
    struct VariableBucket
    {
        using Bucket = std::unordered_map<std::string, Value>;

        void new_var(const std::string& identifier, Value initial_value);
        __maybe_nullptr __non_owning Value* get_var(const std::string& identifier);
    private:

        Bucket _values;
    };
}


#endif //HONK_VARIABLEBUCKET_HPP
