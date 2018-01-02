//
// Created by mafn on 11/9/17.
//

#ifndef HONK_LEXER_HPP
#define HONK_LEXER_HPP

#include <string>
#include <vector>

#include "Lexer/Token.hpp"

namespace Honk
{
    using TokenStream = std::vector<Token>;

    struct Lexer
    {
        Lexer(const std::string& source);

        TokenStream lex_input();

    private:
        std::string _source;
    };
}


#endif //HONK_LEXER_HPP
