//
// Created by mafn on 2/17/18.
//

#include "Parser.hpp"

#include "Interpreter.hpp"
#include "Util.hpp"
#include "AST/Expr.hpp"

namespace Honk
{
    Parser::Parser(const Interpreter& parent, const TokenStream& input)
        : _parent(parent)
        , _tokens(input)
    {
    }

    std::optional<Stmt::stream> Parser::parse_input()
    {
        // Reset parser
        this->_current_token = this->_tokens.begin();

        Stmt::stream statements;
        while (!this->_is_at_end()) {
            statements.push_back(this->_parse_declaration());
        }

        if (this->_has_error) {
            return std::nullopt;
        }

        return statements;
    }

    std::optional<Expr::u_ptr> Parser::parse_input_as_expr()
    {
        // Reset parser
        this->_current_token = this->_tokens.begin();

        Expr::u_ptr expression = this->_parse_expression();

        if (this->_has_error) {
            return std::nullopt;
        }

        return expression;
    }

    Stmt::u_ptr Parser::_parse_declaration()
    {
        try {
            if (this->_match(TokenType::VAR)) {
                return this->_parse_declaration_vardeclaration();
            }

            return this->_parse_statement();
        } catch (const parse_exception& e) {
            // Parsing is fucked, meaning that syntax is in some way incorrect, better synchronise
            this->_synchronise();
            return nullptr;
        }
    }

    Stmt::u_ptr Parser::_parse_declaration_vardeclaration()
    {
        this->_assert_next_token_is(TokenType::IDENTIFIER, PARSER_ERROR::NO_IDENTIFIER_AFTER_VAR);
        Token identifier = this->_get_previous();

        std::optional<Expr::u_ptr> initializer;
        if (this->_match(TokenType::ASSIGNMENT)) {
            initializer = std::move(this->_parse_expression());
        }

        this->_assert_next_token_is(TokenType::SEMICOLON, PARSER_ERROR::UNTERMINATED_VAR);
        return std::make_unique<Stmt::VarDeclaration>(identifier, std::move(initializer));
    }

    Stmt::u_ptr Parser::_parse_statement()
    {
        if (this->_match(TokenType::PRINT)) {
            return this->_parse_statement_print();
        }

        return this->_parse_statement_expression();
    }

    Stmt::u_ptr Parser::_parse_statement_print()
    {
        this->_assert_next_token_is(TokenType::PAREN_OPEN, PARSER_ERROR::PRINT_NO_OPEN);
        Expr::u_ptr expression = this->_parse_expression();
        this->_assert_next_token_is(TokenType::PAREN_CLOSE, PARSER_ERROR::PRINT_NO_CLOSE);
        this->_assert_next_token_is(TokenType::SEMICOLON, PARSER_ERROR::UNTERMINATED_PRINT);

        return std::make_unique<Stmt::Print>(std::move(expression));
    }

    Stmt::u_ptr Parser::_parse_statement_expression()
    {
        Expr::u_ptr expression = this->_parse_expression();

        this->_assert_next_token_is(TokenType::SEMICOLON, PARSER_ERROR::UNTERMINATED_EXPR);

        return std::make_unique<Stmt::Expression>(std::move(expression));
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

        if (this->_match(TokenType::IDENTIFIER)) {
            return std::make_unique<Expr::VarAccess>(this->_get_previous());
        }

        if (this->_match(TokenType::PAREN_OPEN)) {
            Expr::u_ptr grouped_expr = this->_parse_expression();
            this->_assert_next_token_is(TokenType::PAREN_CLOSE, PARSER_ERROR::UNCLOSED_GROUP);

            return std::make_unique<Expr::Grouped>(std::move(grouped_expr));
        }

        this->_panic(PARSER_ERROR::BROKEN_EXPR);
        return nullptr;
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

    // You may be thinking "why not use a default arg?". Because you cannot use the result of a method in it...
    void Parser::_panic(const char* message)
    {
        this->_panic(message, this->_get_previous());
    }

    void Parser::_panic(const char* message, const Token& token)
    {
        this->_has_error = true;
        this->_parent.report_error(token.line, message);

        throw parse_exception(message, token);
    }

    void Parser::_synchronise()
    {
        // TODO: do actual synchronisation. Currently we just consume tokens until the end.
        while (!this->_is_at_end()) {
            this->_advance();
        }
    }

    void Parser::_assert_next_token_is(TokenType type, const char* message)
    {
        if (!this->_match(type)) {
            this->_panic(message);
        }
    }
}