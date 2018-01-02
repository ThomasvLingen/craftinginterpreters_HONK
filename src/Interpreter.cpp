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
        this->_run_code(Util::get_file_contents(path));
    }

    void Interpreter::_run_code(const std::string& source)
    {
        Lexer lexer(source);
        TokenStream tokens = lexer.lex_input();

        // We currently just print the output. Lame...
        for (const Token& token : tokens) {
            std::cout << token.text;
        }
    }
}
