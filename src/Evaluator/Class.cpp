//
// Created by mafn on 7/23/18.
//

#include "Class.hpp"

#include <ostream>

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
}


