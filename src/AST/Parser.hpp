//
// Created by mafn on 2/17/18.
//

#ifndef HONK_PARSER_HPP
#define HONK_PARSER_HPP

#include <vector>

#include "Lexer/Token.hpp"

namespace Honk
{
    // Forward declarations
    struct Interpreter;

    struct Parser
    {
        Parser(const Interpreter& parent, const TokenStream& input);

        // TODO: Make this return the parsed stuff.
        void parse_input();

    private:
        const Interpreter& _parent;
        const TokenStream& _tokens;

        TokenStream::const_iterator _current_token = _tokens.begin();
    };
}

#endif //HONK_PARSER_HPP
