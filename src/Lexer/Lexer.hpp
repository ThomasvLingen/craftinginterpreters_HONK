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
    using KeywordTokenMap = std::unordered_map<std::string, TokenType::_enumerated>;

    struct Lexer
    {
        Lexer(const Interpreter& parent, const std::string& source);

        // If there is an error in the token parsing, no tokens are returned
        std::optional<TokenStream> lex_input();

    private:
        static SingleCharTokenMap single_char_tokens;
        static KeywordTokenMap keyword_tokens;
        const Interpreter& _honk;

        std::string _source;
        TokenStream _tokens;

        std::string::const_iterator _token_start;
        std::string::const_iterator _current_char;
        uint32_t _current_line = 1;
        bool _has_error = false; // When an error is encountered, this gets set to true

        char _advance();
        char _peek();
        void _lex_token();

        template <typename Callable>
        bool _match(Callable comparator)
        {
            if (this->_is_at_end()) {
                return false;
            }

            if (comparator(*this->_current_char)) {
                this->_advance();
                return true;
            } else {
                return false;
            }
        }
        bool _match(char to_match);
        void _advance_until(char to_match);

        bool _add_singlechar_token(char c);
        bool _add_single_or_double(char c, char single_c, char double_c, TokenType single_type, TokenType double_type);
        bool _add_string_literal(char c);
        bool _add_integer_literal(char c);
        bool _add_ident_or_keyword(char c);
        bool _add_bool_literal(const std::string& token_text);
        bool _add_keyword(const std::string& token_text);

        void _add_token(TokenType type, TokenLiteral value = "");

        std::string _get_token_text();
        bool _is_at_end();

        void _error_unknown_char(char c);
        void _error_unclosed_string();
    };
}


#endif //HONK_LEXER_HPP
