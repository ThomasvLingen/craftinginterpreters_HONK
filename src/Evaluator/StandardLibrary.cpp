//
// Created by mafn on 4/2/18.
//

#include "StandardLibrary.hpp"

#include <chrono>
#include <iostream>

#include "Evaluator.hpp"

namespace Honk
{
    std::vector<NativeCallable> StandardLibrary::Native::_fns {
        {"print"               , StandardLibrary::Native::print          , 1},
        {"println"             , StandardLibrary::Native::println        , 1},
        {"honk_get_scope_depth", StandardLibrary::Native::get_scope_depth, 0},
        {"honk_get_time_ms"    , StandardLibrary::Native::get_time_ms    , 0}
    };

    Value StandardLibrary::Native::get_scope_depth(Evaluator& runtime, Arguments)
    {
        honk_int_t scope_depth = runtime.scopes.get_scope_depth();
        return Value{scope_depth};
    }

    Value StandardLibrary::Native::get_time_ms(Evaluator&, Arguments)
    {
        using namespace std::chrono;

        milliseconds time_ms = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        );
        honk_int_t ms = time_ms.count();

        return Value {ms};
    }

    std::string _get_printable(Value& val)
    {
        if (bool* val_bool = val.get<bool>()) {
            return Util::bool_str(*val_bool);
        }

        return _to_string(val.value);
    }

    Value StandardLibrary::Native::print(Evaluator&, Arguments args)
    {
        std::cout << _get_printable(args[0]);

        return { null };
    }

    Value StandardLibrary::Native::println(Evaluator& runtime, Arguments args)
    {
        std::cout << _get_printable(args[0]) << std::endl;

        return { null };
    }

    void StandardLibrary::register_in(VariableBucket::Scoped& scopes)
    {
        VariableBucket& globals = scopes.get_global_env();

        for (NativeCallable& fn : StandardLibrary::Native::_fns) {
            globals.new_var(fn);
        }
    }
}