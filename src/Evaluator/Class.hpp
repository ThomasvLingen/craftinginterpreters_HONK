//
// Created by mafn on 7/23/18.
//

#ifndef HONK_CLASS_HPP
#define HONK_CLASS_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "AST/Callable.hpp"
#include "Evaluator/Function.hpp"

namespace Honk
{
    struct ClassInstance;

    // NOTE: This is a class _declaration_. Not an instance. For that, see `ClassInstance.hpp`
    struct Class : Callable
    {
        // NOTE: This does mean that the identifier actually gets stored _twice_.
        //       Once in the Function object, and once in the map key.
        using MethodMap = std::unordered_map<std::string /*name*/, Function /*method*/>;

        Class(std::string name, std::vector<std::string> fields, MethodMap methods);

        // Serves as a constructor
        Value call(Evaluator& runtime, Arguments args);
        size_t n_args() const;

        std::shared_ptr<Value> get_bound_method(std::shared_ptr<ClassInstance> instance, std::string identifier) const;
        std::shared_ptr<Value> get_method(std::string identifier) const;

        std::string name;
        std::vector<std::string> declared_fields;
        MethodMap declared_methods;

        friend std::ostream& operator<<(std::ostream& os, const Class& obj);
        friend bool operator==(const Class& a, const Class& b);
    };
}


#endif //HONK_CLASS_HPP
