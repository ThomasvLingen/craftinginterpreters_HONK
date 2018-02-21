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

    struct Expr;

    struct Interpreter
    {
        Interpreter(bool debug);

        void run_from_file(const string& path);
        void run_repl();

        void report_error(uint32_t line, const string& message) const;
        void report_message(const string& type, uint32_t line, const string& message) const;

    private:
        bool _debug = false;
        string _current_file = "";

        void _run_code(const string& source);
        void _print_tokens(const TokenStream& tokens);
        // TODO: This should logically be const, but it isn't since the Visitor interface doesn't stick to it.
        void _print_AST(Expr& expr);
    };
}

#endif //HONK_INTERPRETER_HPP
