//
// Created by mafn on 1/2/18.
//

#ifndef HONK_INTERPRETER_HPP
#define HONK_INTERPRETER_HPP

#include <string>

namespace Honk
{
    struct Interpreter
    {
        void run_from_file(const std::string& path);
        void run_repl();

    private:
        void _run_code(const std::string& source);
    };
}

#endif //HONK_INTERPRETER_HPP
