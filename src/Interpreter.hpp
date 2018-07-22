//
// Created by mafn on 1/2/18.
//

#ifndef HONK_INTERPRETER_HPP
#define HONK_INTERPRETER_HPP

#include <string>

#include "AST/Visitors/PrettyPrinter.hpp"
#include "AST/Stmt.hpp"
#include "Evaluator/Evaluator.hpp"
#include "Lexer/Token.hpp"
#include "Resolver/Resolver.hpp"

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
        Evaluator _evaluator;
        PrettyASTPrinter _printer;

        bool _debug = false;
        string _current_file = "";

        void _run_code(const string& source);
        void _print_tokens(const TokenStream& tokens);
        // TODO: This should logically be const, but it isn't since the Visitor interface doesn't stick to it.
        void _print_expression(Expr& expr);
        void _print_statements(Stmt::stream& statements);
        void _print_resolvemapping(VariableResolveMapping& resolved);
    };
}

#endif //HONK_INTERPRETER_HPP
