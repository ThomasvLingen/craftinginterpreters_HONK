//
// Created by mafn on 11/9/17.
//

#include "Lexer.hpp"

#include <iostream>
#include <sstream>
#include <optional>

#include "Interpreter.hpp"
#include "Util.hpp"

namespace Honk
{
    SingleCharTokenMap Lexer::single_char_tokens = {
        {'(', TokenType::PAREN_OPEN}, {')', TokenType::PAREN_CLOSE},
        {'{', TokenType::CURLY_OPEN}, {'}', TokenType::CURLY_CLOSE},
        {',', TokenType::COMMA},      {';', TokenType::SEMICOLON},
        {'+', TokenType::PLUS},       {'-', TokenType::MINUS},
        {'*', TokenType::STAR},       {'/', TokenType::SLASH},
    };

    KeywordTokenMap Lexer::keyword_tokens = {
        {"fun",   TokenType::FUN},   {"return", TokenType::RETURN},
        {"const", TokenType::CONST}, {"var",    TokenType::VAR},
        {"if",    TokenType::IF},    {"else",   TokenType::ELSE},   {"while", TokenType::WHILE},    {"for", TokenType::FOR},
        {"and",   TokenType::AND},   {"or",     TokenType::OR},
        {"print", TokenType::PRINT},
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

        this->_add_token(Token {TokenType::END_OF_FILE, "", "", this->_current_line});

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

        // String literals
        if (this->_add_string_literal(c)) {
            return;
        }

        // Integer literals
        if (this->_add_integer_literal(c)) {
            return;
        }

        // Identifiers & keywords
        if (this->_add_ident_or_keyword(c)) {
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

    void Lexer::_add_token(Token tok)
    {
        this->_tokens.push_back(tok);
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

        this->_honk.report_error(this->_current_line, error_message.str());
    }

    void Lexer::_error_unclosed_string()
    {
        this->_has_error = true;

        std::string unclosed_string = this->_get_token_text();
        std::stringstream error_message;
        error_message << "Unclosed string: " << unclosed_string;

        this->_honk.report_error(this->_current_line, error_message.str());
    }

    bool Lexer::_match(char to_match)
    {
        return this->_match([&to_match] (char c) {
            return c == to_match;
        });
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

    // This is a bit messy. Maybe re-do.
    // Maybe _match could be used better here.
    bool Lexer::_add_string_literal(char c)
    {
        if (c != '"') {
            return false;
        }

        // Edge case, immediate newline
        if (this->_is_at_end() || *this->_current_char == '\n') {
            this->_error_unclosed_string();
            return true;
        }

        while (*this->_current_char != '"') {
            if (this->_peek() == '\n' || this->_is_at_end()) {
                this->_advance();
                this->_error_unclosed_string();
                return true;
            }

            this->_advance();
        }

        this->_advance(); // Grab the closing " as well

        std::string string_text = this->_get_token_text();
        this->_add_token(TokenType::STRING, string_text.substr(1, string_text.size() - 2));
        return true;
    }

    bool Lexer::_add_integer_literal(char c)
    {
        if (!isdigit(c)) {
            return false;
        }

        while (this->_match(isdigit)) {
            // Just keep eating the number
        }

        this->_add_token(TokenType::INT, std::stoi(this->_get_token_text()));
        return true;
    }

    bool isidentifier(char c) {
        return isalnum(c) || c == '_';
    }

    bool Lexer::_add_ident_or_keyword(char c)
    {
        if (!isidentifier(c)) {
            return false;
        }

        while (this->_match(isidentifier)) {
            // Eat the identifier...
        }

        std::string token_text = this->_get_token_text();

        // Is it a bool, null, or a keyword?
        if (this->_add_bool_literal(token_text)
            || this->_add_null_literal(token_text)
            || this->_add_keyword(token_text))
        {
            return true;
        }

        // Ok, then it's just a regular identifier
        this->_add_token(TokenType::IDENTIFIER, token_text);
        return true;
    }

    static std::unordered_map<std::string, bool> _bool_map = {
        {"true", true}, {"false", false}
    };

    bool Lexer::_add_bool_literal(const std::string& token_text)
    {
        std::optional<bool> bool_value = Util::map_get_optional(_bool_map, token_text);

        if (!bool_value) {
            return false;
        }

        this->_add_token(TokenType::BOOL, bool_value.value());
        return true;
    }

    bool Lexer::_add_null_literal(const std::string& token_text)
    {
        if (token_text != "null") {
            return false;
        }

        this->_add_token(TokenType::VAL_NULL, null);
        return true;
    }

    bool Lexer::_add_keyword(const std::string& token_text)
    {
        std::optional<TokenType> keyword_type = Util::map_get_optional(Lexer::keyword_tokens, token_text);

        if (!keyword_type) {
            return false;
        }

        this->_add_token(keyword_type.value());
        return true;
    }
}
