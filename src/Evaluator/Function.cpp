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
    Function::Function(std::optional<std::string> identifier, Expr::Fun* declaration,
                       std::shared_ptr<VariableBucket> closure) noexcept
        : _identifier(identifier)
        , _declaration(declaration)
        , _closure(closure)
    {
    }

    size_t Function::n_args() const
    {
        return this->_declaration->parameters.size();
    }

    Value Function::call(Evaluator& runtime, Arguments args)
    {
        Util::ScopeGuard<VariableBucket*> fn_scope(runtime.scopes, this->_closure.get());
        this->_define_args_in_bucket(runtime.env(), args);

        try {
            runtime.execute_block(this->_declaration->get_body());
        } catch(Return& return_stmt) {
            return return_stmt.returned_value;
        }

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

    Function Function::bind(ClassInstance::s_ptr instance)
    {
        auto bound_closure = std::make_shared<VariableBucket>(this->_closure.get());
        bound_closure->new_var("this", Value {instance});

        return Function(this->_identifier, this->_declaration, bound_closure);
    }

    std::ostream& operator<<(std::ostream& os, const Function& obj)
    {
        if (obj._identifier) {
            os << "fun:" << *obj._identifier;
        } else {
            os << "anon_fn";
        }

        os << "("    << obj.n_args() << " args)";

        return os;
    }

    bool operator==(const Function& a, const Function& b)
    {
        return a.n_args() == b.n_args() &&
               &a._declaration == &b._declaration;
    }
}
