//
// Created by mafn on 1/2/18.
//

#ifndef HONK_INTERPRETER_HPP
#define HONK_INTERPRETER_HPP

#include <string>

namespace Honk
{
    using std::string;

    struct Interpreter
    {
        void run_from_file(const string& path);
        void run_repl();

        void fuck(uint32_t line, const string& message) const;
        void report_message(const string& type, uint32_t line, const string& message) const;

    private:
        string _current_file = "";

        void _run_code(const string& source);
    };
}

#endif //HONK_INTERPRETER_HPP
