//
// Created by mafn on 1/2/18.
//

#include "Interpreter.hpp"

#include <iostream>
#include <fstream>

#include "Util/Util.hpp"
#include "Lexer/Lexer.hpp"
#include "AST/Parser.hpp"
#include "AST/Visitors/PrettyPrinter.hpp"
#include "Evaluator/Evaluator.hpp"

namespace Honk
{
    Interpreter::Interpreter(bool debug)
//        : _evaluator(*this)
//        , _debug(debug)
        : _debug(debug)
    {
    }

    void Interpreter::run_repl()
    {
        this->_current_file = "REPL";

        while (true) {
            std::string line;
            std::cout << "==> ";
            std::getline(std::cin, line);

            if (line == "quit!") {
                break;
            }

            this->_run_code(line);
        }
    }

    void Interpreter::run_from_file(const std::string& path)
    {
        this->_current_file = path;
        this->_run_code(Util::get_file_contents(path));
    }

    void Interpreter::_run_code(const std::string& source)
    {
        // Lex text input into TokenStream
        Lexer lexer(*this, source);
        std::optional<TokenStream> tokens = lexer.lex_input();

        if (!tokens) {
            // lol not going to run that
            return;
        }

        if (this->_debug) {
            this->_print_tokens(tokens.value());
        }

        // Parse TokenStream into AST
        Parser parser(*this, tokens.value());
        std::optional<Stmt::stream> AST = parser.parse_input();
        if (!AST) {
            // lol not going to run that
            return;
        }

        if (this->_debug) {
            this->_print_statements(*AST);
        }

        // Run the code!
        // this->_evaluator.interpret(*AST);
    }

    void Interpreter::report_message(const string& type, uint32_t line, const string& message) const
    {
        std::cout << "[" << type << "] "
                  << this->_current_file << ":" << line << " "
                  << message << "\n";
    }

    void Interpreter::report_error(uint32_t line, const string& message) const
    {
        this->report_message("ERROR", line, message);
    }

    void Interpreter::_print_tokens(const TokenStream& tokens)
    {
        for (const Token& token : tokens) {
            std::cout << token.to_str() << '\n';
        }
    }

    void Interpreter::_print_expression(Expr& expr)
    {
         this->_printer.print(expr);
    }

    void Interpreter::_print_statements(Stmt::stream& statements)
    {
        for (Stmt::u_ptr& statement : statements) {
            this->_printer.print(*statement);
        }
    }
}
