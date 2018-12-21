//
// Created by mafn on 7/25/18.
//

#include "ClassInstance.hpp"

#include "Class.hpp"

#include "Util/Util.hpp"
#include "AST/Value.hpp"

namespace Honk
{
    ClassInstance::ClassInstance(const Class& cls)
        : _class(&cls)
    {
    }

    std::ostream& operator<<(std::ostream& os, const ClassInstance& obj)
    {
        os << "instance_of:" << *obj._class;

        return os;
    }

    bool operator==(const ClassInstance& a, const ClassInstance& b)
    {
        return &a == &b;
    }

    Value::s_ptr ClassInstance::get_field(std::string identifier)
    {
        if (!Util::contains(this->_fields, identifier)) {
            return nullptr;
        }

        return Util::map_get_s_ptr(this->_fields, identifier);
    }

    void ClassInstance::set_field(std::string identifier, std::shared_ptr<Value> value)
    {
        // TODO: Make it so this checks if the field was declared in the classdecl.
        this->_fields[identifier] = value;
    }
}