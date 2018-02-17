//
// Created by mafn on 1/2/18.
//

#include "Interpreter.hpp"

#include <iostream>
#include <fstream>

#include "Util.hpp"
#include "Lexer/Lexer.hpp"
#include "AST/Parser.hpp"

namespace Honk
{
    Interpreter::Interpreter(bool debug)
        : _debug(debug)
    {
    }

    void Interpreter::run_repl()
    {
        this->_current_file = "REPL";

        while (true) {
            std::string line;
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
        parser.parse_input();
    }

    void Interpreter::report_message(const string& type, uint32_t line, const string& message) const
    {
        std::cout << "[" << type << "] "
                  << this->_current_file << ":" << line << " "
                  << message << "\n";
    }

    void Interpreter::fuck(uint32_t line, const string& message) const
    {
        this->report_message("ERROR", line, message);
    }

    void Interpreter::_print_tokens(const TokenStream& tokens)
    {
        for (const Token& token : tokens) {
            std::cout << token.to_str() << '\n';
        }
    }
}
