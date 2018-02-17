//
// Created by mafn on 1/2/18.
//

#ifndef HONK_INTERPRETER_HPP
#define HONK_INTERPRETER_HPP

#include <string>

#include "Lexer/Token.hpp"

namespace Honk
{
    using std::string;

    struct Interpreter
    {
        Interpreter(bool debug);

        void run_from_file(const string& path);
        void run_repl();

        void fuck(uint32_t line, const string& message) const;
        void report_message(const string& type, uint32_t line, const string& message) const;

    private:
        bool _debug = false;
        string _current_file = "";

        void _run_code(const string& source);
        void _print_tokens(const TokenStream& tokens);
    };
}

#endif //HONK_INTERPRETER_HPP
