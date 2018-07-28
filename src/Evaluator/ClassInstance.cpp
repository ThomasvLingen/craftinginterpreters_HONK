//
// Created by mafn on 7/25/18.
//

#include "ClassInstance.hpp"

#include "Class.hpp"

namespace Honk
{
    ClassInstance::ClassInstance(const Class& cls)
        : _class(&cls)
    {
    }

    std::ostream& operator<<(std::ostream& os, const ClassInstance& obj)
    {
        // BUG: fixme, currently hella broken because of the way Values are instantiated / passed around.
        os << "instanceof:" << obj._class->name;

        return os;
    }

    bool operator==(const ClassInstance& a, const ClassInstance& b)
    {
        return &a == &b;
    }
}