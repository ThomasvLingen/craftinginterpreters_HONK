//
// Created by mafn on 2/17/18.
//

#include "Parser.hpp"

#include "Interpreter.hpp"
#include "Util.hpp"
#include "AST/Expr.hpp"
#include "AST/PrettyPrinter.hpp"

namespace Honk
{
    Parser::Parser(const Interpreter& parent, const TokenStream& input)
        : _parent(parent)
        , _tokens(input)
    {
    }

    std::optional<Expr::u_ptr> Parser::parse_input()
    {
        // Reset parser
        this->_current_token = this->_tokens.begin();

        Expr::u_ptr expression = this->_parse_expression();
        if (this->_has_error) {
            return std::nullopt;
        }

        return std::move(expression);
    }

    Expr::u_ptr Parser::_parse_expression()
    {
        return this->_parse_equality();
    }

    bool _match_equality(const Token& token)
    {
        static auto match_tokentypes = {TokenType::EQUALS, TokenType::NOT_EQUALS};
        return Util::contains(match_tokentypes, token.type);
    }

    Expr::u_ptr Parser::_parse_equality()
    {
        Expr::u_ptr left = this->_parse_comparison();

        while (this->_match(_match_equality)) {
            Token op = this->_get_previous();
            Expr::u_ptr right = this->_parse_comparison();
            left = std::make_unique<Expr::Binary>(std::move(left), op, std::move(right));
        }

        return left;
    }

    bool _match_comparison(const Token& token)
    {
        static auto match_tokentypes = {TokenType::GREATER, TokenType::GREATER_EQ, TokenType::LESS_THAN, TokenType::LESS_THAN_EQ};
        return Util::contains(match_tokentypes, token.type);
    }

    Expr::u_ptr Parser::_parse_comparison()
    {
        Expr::u_ptr left = this->_parse_addition();

        while (this->_match(_match_comparison)) {
            Token op = this->_get_previous();
            Expr::u_ptr right = this->_parse_addition();
            left = std::make_unique<Expr::Binary>(std::move(left), op, std::move(right));
        }

        return left;
    }

    bool _match_addition(const Token& token)
    {
        static auto match_tokentypes = {TokenType::MINUS, TokenType::PLUS};
        return Util::contains(match_tokentypes, token.type);
    }

    Expr::u_ptr Parser::_parse_addition()
    {
        Expr::u_ptr left = this->_parse_multiplication();

        while (this->_match(_match_addition)) {
            Token op = this->_get_previous();
            Expr::u_ptr right = this->_parse_multiplication();
            left = std::make_unique<Expr::Binary>(std::move(left), op, std::move(right));
        }

        return left;
    }

    bool _match_multiplication(const Token& token)
    {
        static auto match_tokentypes = {TokenType::STAR, TokenType::SLASH};
        return Util::contains(match_tokentypes, token.type);
    }

    Expr::u_ptr Parser::_parse_multiplication()
    {
        Expr::u_ptr left = this->_parse_unary();

        while (this->_match(_match_multiplication)) {
            Token op = this->_get_previous();
            Expr::u_ptr right = this->_parse_unary();
            left = std::make_unique<Expr::Binary>(std::move(left), op, std::move(right));
        }

        return left;
    }

    bool _match_unary(const Token& token)
    {
        static auto match_tokentypes = {TokenType::NOT, TokenType::MINUS};
        return Util::contains(match_tokentypes, token.type);
    }

    Expr::u_ptr Parser::_parse_unary()
    {
        if (!this->_match(_match_unary)) {
            return this->_parse_primary();
        }

        Token op = this->_get_previous();
        return std::make_unique<Expr::Unary>(op, this->_parse_unary());
    };

    bool _match_literal(const Token& token)
    {
        static auto match_tokentypes = {TokenType::BOOL, TokenType::INT, TokenType::STRING, TokenType::VAL_NULL};
        return Util::contains(match_tokentypes, token.type);
    }

    Expr::u_ptr Parser::_parse_primary()
    {
        if (this->_match(_match_literal)) {
            TokenLiteral value = this->_get_previous().value;
            return std::make_unique<Expr::Literal>(value);
        }

        if (this->_match(TokenType::PAREN_OPEN)) {
            Expr::u_ptr grouped_expr = this->_parse_expression();
            if (!this->_match(TokenType::PAREN_CLOSE)) {
                this->_error_unclosed_param();
                return this->_error_expr();
            }
            return std::make_unique<Expr::Grouped>(std::move(grouped_expr));
        }

        this->_error_broken_expression();
        return this->_error_expr();
    }

    const Token& Parser::_advance()
    {
        return *(this->_current_token)++;
    }

    bool Parser::_is_at_end()
    {
        return this->_get_current().type == TokenType::END_OF_FILE;
    }

    template <typename Callable>
    bool Parser::_match(Callable comparator)
    {
        if (this->_is_at_end()) {
            return false;
        }

        if (comparator(this->_get_current())) {
            this->_advance();
            return true;
        } else {
            return false;
        }
    }

    bool Parser::_match(TokenType type)
    {
        return this->_match([&type] (const Token& current) {
            return current.type == type;
        });
    }

    const Token& Parser::_get_current()
    {
        return *this->_current_token;
    }

    bool Parser::_match(std::initializer_list<TokenType> types)
    {
        return this->_match([&types] (const Token& token) {
            return Util::contains(types, token.type);
        });
    }

    const Token& Parser::_get_previous()
    {
        return *std::prev(this->_current_token);
    }

    Expr::u_ptr Parser::_error_expr()
    {
        return std::make_unique<Expr::Literal>(std::string("lol broken"));
    }

    void Parser::_error_broken_expression()
    {
        this->_has_error = true;

        this->_parent.report_error(this->_get_current().line, "An expression is broken (unrecognised symbols)");
    }

    void Parser::_error_unclosed_param()
    {
        this->_has_error = true;

        this->_parent.report_error(this->_get_current().line, "An opening parenthesis '(' was not closed");
    }
}