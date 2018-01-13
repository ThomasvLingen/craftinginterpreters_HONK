//
// Created by mafn on 11/9/17.
//

#ifndef HONK_LEXER_HPP
#define HONK_LEXER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include "Lexer/Token.hpp"

namespace Honk
{
    class Interpreter;

    using TokenStream = std::vector<Token>;
    using SingleCharTokenMap = std::unordered_map<char, TokenType::_enumerated>;

    struct Lexer
    {
        Lexer(const Interpreter& parent, const std::string& source);

        // If there is an error in the token parsing, no tokens are returned
        std::optional<TokenStream> lex_input();

    private:
        static SingleCharTokenMap single_char_tokens;
        const Interpreter& _honk;

        std::string _source;
        TokenStream _tokens;

        std::string::const_iterator _token_start;
        std::string::const_iterator _current_char;
        uint32_t _current_line = 1;
        bool _has_error = false; // When an error is encountered, this gets set to true

        char _advance();
        void _lex_token();

        std::optional<TokenType> _get_singlechar_token(char c);

        void _add_token(TokenType type, TokenLiteral value = "");

        std::string _get_token_text();
        bool _is_at_end();

        void _error_unknown_char(char c);
    };
}


#endif //HONK_LEXER_HPP
