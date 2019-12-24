//
// Created by mafn on 7/23/18.
//

#include "Class.hpp"

#include <ostream>

#include "Util/Util.hpp"

#include "AST/Value.hpp"
#include "ClassInstance.hpp"

namespace Honk
{
    Class::Class(std::string name, std::vector<std::string> fields, MethodMap methods)
        : name(name)
        , declared_fields(fields)
        , declared_methods(methods)
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
        ClassInstance::s_ptr instance = std::make_shared<ClassInstance>(*this);

        // NOTE: we use a std::string literal here since for some arcane reason using a normal string literal makes template argument deduction shit itself
        using namespace std::string_literals;
        if (Util::contains(this->declared_methods, "init"s)) {
            this->declared_methods.at("init").bind(instance).call(runtime, args);  // TODO refactor this into an optional
        }

        return Value {instance};
    }

    size_t Class::n_args() const
    {
        using namespace std::string_literals;
        if (Util::contains(this->declared_methods, "init"s)) {
            return this->declared_methods.at("init").n_args();
        } else {
            return 0;
        }
    }

    std::shared_ptr<Value> Class::get_bound_method(ClassInstance::s_ptr instance, std::string identifier) const
    {
        if (!Util::contains(this->declared_methods, identifier)) {
            return nullptr;
        }

        Function unbound_method = this->declared_methods.at(identifier);
        return std::make_shared<Value>(unbound_method.bind(instance));
    }

    std::shared_ptr<Value> Class::get_method(std::string identifier) const
    {
        if (Util::contains(this->declared_methods, identifier)) {
            // NOTE: This means we make a copy here and detach it.
            return std::make_shared<Value>(this->declared_methods.at(identifier));
        } else {
            return nullptr;
        }
    }
}


