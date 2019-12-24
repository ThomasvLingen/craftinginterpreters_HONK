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
        this->_declare_instance_fields();
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
        // Do we have a field that matches it?
        if (Util::contains(this->_fields, identifier)) {
            return Util::map_get_s_ptr(this->_fields, identifier);
        }

        // If not, check if Class has a method that matches it
        // BIND IT
        return this->_class->get_bound_method(shared_from_this(), identifier);
    }

    bool ClassInstance::set_field(std::string identifier, std::shared_ptr<Value> value)
    {
        if (!Util::contains(this->_fields, identifier)) {
            return false;
        }

        this->_fields[identifier] = value;
        return true;
    }

    void ClassInstance::_declare_instance_fields()
    {
        for (const std::string& field : this->_class->declared_fields) {
            this->_declare_field(field);
        }
    }

    void ClassInstance::_declare_field(std::string identifier)
    {
        this->_fields[identifier] = std::make_shared<Value>(null);
    }
}