//
// Created by mafn on 11/9/17.
//

#include "Lexer.hpp"

#include <iostream>
#include <sstream>

#include "Interpreter.hpp"

namespace Honk
{
    SingleCharTokenMap Lexer::single_char_tokens = {
        {'(', (+TokenType::PAREN_OPEN)},  {')', (+TokenType::PAREN_CLOSE)},
        {'{', (+TokenType::CURLY_OPEN)},  {'}', (+TokenType::CURLY_CLOSE)},
        {',', (+TokenType::COMMA)},
        {';', (+TokenType::SEMICOLON)},
        {'+', (+TokenType::PLUS)}, {'-', (+TokenType::MINUS)},
        {'*', (+TokenType::STAR)}, {'/', (+TokenType::SLASH)},
    };

    Lexer::Lexer(const Interpreter& parent, const std::string& source)
    :_honk(parent)
    , _source(source)
    , _current_char(this->_source.begin())
    {
    }

    std::optional<TokenStream> Lexer::lex_input()
    {
        // Reset lexing
        this->_current_line = 1;
        this->_current_char = this->_source.begin();

        while (!this->_is_at_end()) {
            this->_token_start = this->_current_char;
            this->_lex_token();
        }

        if (this->_has_error) {
            return std::nullopt;
        }

        return this->_tokens;
    }

    bool Lexer::_is_at_end()
    {
        return this->_current_char == this->_source.end();
    }

    void Lexer::_lex_token()
    {
        char c = this->_advance();

        // Whitespace is ignored
        if (isblank(c)) {
            return;
        }

        // Newlines = next line
        if (c == '\n') {
            this->_current_line++;
            return;
        }

        // Comment = skip to end of line
        if (c == '/' && _match('/')) {
            this->_advance_until('\n');
            return;
        }

        // Single character tokens
        if (this->_add_singlechar_token(c)) {
            return;
        }

        // Not + double char tokens
        if (this->_add_single_or_double(c, '!', '=', TokenType::NOT, TokenType::NOT_EQUALS)
            || this->_add_single_or_double(c, '=', '=', TokenType::ASSIGNMENT, TokenType::EQUALS)
            || this->_add_single_or_double(c, '<', '=', TokenType::LESS_THAN, TokenType::LESS_THAN_EQ)
            || this->_add_single_or_double(c, '>', '=', TokenType::GREATER, TokenType::GREATER_EQ))
        {
            return;
        }

        // Well, I don't know what to do with this.
        this->_error_unknown_char(c);
    }

    void Lexer::_add_token(TokenType type, TokenLiteral value)
    {
        this->_tokens.push_back(Token {
            type, this->_get_token_text(), value, this->_current_line
        });
    }

    std::string Lexer::_get_token_text()
    {
        return std::string(this->_token_start, this->_current_char);
    }

    char Lexer::_advance()
    {
        return *(this->_current_char)++;
    }

    void Lexer::_error_unknown_char(char c)
    {
        this->_has_error = true;

        std::stringstream error_message;
        error_message <<  "Unknown character \'" << c << "\', wtf is this?";

        this->_honk.fuck(this->_current_line, error_message.str());
    }

    bool Lexer::_match(char to_match)
    {
        if (this->_is_at_end()) {
            return false;
        }

        if (*this->_current_char == to_match) {
            this->_current_char++;
            return true;
        } else {
            return false;
        }
    }

    bool Lexer::_add_singlechar_token(char c)
    {
        SingleCharTokenMap& token_map = Lexer::single_char_tokens;

        if (token_map.find(c) == token_map.end()) {
            return false;
        }

        this->_add_token(token_map[c]);
        return true;
    }

    bool Lexer::_add_single_or_double(char c, char single_c, char double_c, TokenType single_type, TokenType double_type)
    {
        if (c != single_c) {
            return false;
        }

        this->_add_token(
            this->_match(double_c) ? double_type : single_type
        );

        return true;
    }

    void Lexer::_advance_until(char to_match)
    {
        while (this->_peek() != to_match && !this->_is_at_end()) {
            this->_advance();
        }

        this->_advance(); // Get on the matched char
    }

    char Lexer::_peek()
    {
        if (this->_is_at_end()) {
            return '\0';
        }

        return *(std::next(this->_current_char));
    }
}
