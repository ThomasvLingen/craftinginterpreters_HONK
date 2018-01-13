//
// Created by mafn on 1/2/18.
//

#include "Interpreter.hpp"

#include <iostream>
#include <fstream>

#include "Util.hpp"
#include "Lexer/Lexer.hpp"

namespace Honk
{
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
        Lexer lexer(*this, source);
        std::optional<TokenStream> tokens = lexer.lex_input();

        if (!tokens) {
            // lol not going to run that
            return;
        }

        // We currently just print the output. Lame...
        for (const Token& token : tokens.value()) {
            std::cout << token.to_str();
        }
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
}
