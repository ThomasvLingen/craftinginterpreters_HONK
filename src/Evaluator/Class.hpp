//
// Created by mafn on 7/23/18.
//

#ifndef HONK_CLASS_HPP
#define HONK_CLASS_HPP

#include <string>

#include "AST/Callable.hpp"

namespace Honk
{
    struct Class : Callable
    {
        Class(std::string name);

        // Serves as a constructor
        Value call(Evaluator& runtime, Arguments args);
        size_t n_args() const;

        std::string name;

        friend std::ostream& operator<<(std::ostream& os, const Class& obj);
        friend bool operator==(const Class& a, const Class& b);
    };
}


#endif //HONK_CLASS_HPP
