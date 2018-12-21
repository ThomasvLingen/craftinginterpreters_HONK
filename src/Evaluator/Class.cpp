//
// Created by mafn on 7/23/18.
//

#include "Class.hpp"

#include <ostream>

#include "AST/Value.hpp"
#include "ClassInstance.hpp"

namespace Honk
{
    Class::Class(std::string name)
        : name(name)
    {
    }

    std::ostream& operator<<(std::ostream& os, const Class& obj)
    {
        os << "class:" << obj.name;

        return os;
    }

    bool operator==(const Class& a, const Class& b)
    {
        // We compare by identity
        return &a == &b;
    }

    Value Class::call(Evaluator& runtime, Arguments args)
    {
        // TODO finish this
        return Value {
            std::make_shared<ClassInstance>(*this)
        };
    }

    size_t Class::n_args() const
    {
        // TODO finish this
        return 0;
    }
}


