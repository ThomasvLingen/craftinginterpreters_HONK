//
// Created by mafn on 11/9/17.
//

#ifndef HONK_LEXER_HPP
#define HONK_LEXER_HPP

#include <string>
#include <vector>
#include <unordered_map>

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
        char _peek();
        bool _match(char to_match);
        void _advance_until(char to_match);
        void _lex_token();

        bool _add_singlechar_token(char c);
        bool _add_single_or_double(char c, char single_c, char double_c, TokenType single_type, TokenType double_type);

        void _add_token(TokenType type, TokenLiteral value = "");

        std::string _get_token_text();
        bool _is_at_end();

        void _error_unknown_char(char c);
    };
}


#endif //HONK_LEXER_HPP
