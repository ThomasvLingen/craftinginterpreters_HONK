//
// Created by mafn on 4/15/18.
//

#include "Function.hpp"

#include "Util/Util.hpp"
#include "Evaluator/Evaluator.hpp"
#include "AST/Value.hpp"
#include "Evaluator/VariableBucket.hpp"

namespace Honk
{
    Function::Function(Stmt::FunDeclaration* declaration) noexcept
        : _declaration(declaration)
    {
    }

    size_t Function::n_args() const
    {
        return this->_declaration->parameters.size();
    }

    Value Function::call(Evaluator& runtime, Arguments args)
    {
        Util::ScopeGuard<VariableBucket*> fn_scope(runtime.scopes, &runtime.globals);
        this->_define_args_in_bucket(runtime.env(), args);

        runtime.execute_block(this->_declaration->get_body());

        return Value {null};
    }

    void Function::_define_args_in_bucket(VariableBucket& env, Arguments& args)
    {
        for (size_t arg_i = 0; arg_i < args.size(); arg_i++) {
            const std::string& identifier = this->_declaration->parameters[arg_i];
            Value& value = args[arg_i];

            env.new_var(identifier, value);
        }
    }

    std::ostream& operator<<(std::ostream& os, const Function& obj)
    {
        os << "fun:" << obj._declaration->identifier.text
           << "("    << obj.n_args() << " args)";

        return os;
    }

    bool operator==(const Function& a, const Function& b)
    {
        return a.n_args() == b.n_args() &&
               a._declaration->identifier.text == b._declaration->identifier.text;
    }
}
