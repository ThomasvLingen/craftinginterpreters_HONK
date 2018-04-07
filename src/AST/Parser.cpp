//
// Created by mafn on 2/17/18.
//

#include "Parser.hpp"

#include "Interpreter.hpp"
#include "Util/Util.hpp"
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
            statements.push_back(this->_parse_protected_decl());
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


    Stmt::u_ptr Parser::_parse_protected_decl()
    {
        try {
            return this->_parse_declaration();
        } catch (const parse_exception& e) {
            // Parsing is fucked, meaning that syntax is in some way incorrect, better synchronise
            this->_synchronise();
            return nullptr;
        }
    }

    Stmt::u_ptr Parser::_parse_declaration()
    {
        if (this->_match(TokenType::VAR)) {
            return this->_parse_declaration_vardeclaration();
        }

        if (this->_match(TokenType::FUN)) {
            return this->_parse_declaration_fun();
        }

        return this->_parse_statement();
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

    // fun keyword is parsed
    Stmt::u_ptr Parser::_parse_declaration_fun()
    {
        Token identifier = this->_assert_match(TokenType::IDENTIFIER, PARSER_ERROR::FUN::NO_IDENTIFIER);

        this->_assert_next_token_is(TokenType::PAREN_OPEN, PARSER_ERROR::FUN::NO_PARAMS);
        std::vector<Token> param_tokens = this->_parse_parameters();
        if (param_tokens.size() > this->_MAX_CALL_ARGS) {
            this->_panic(PARSER_ERROR::PARAMS::TOO_MANY, param_tokens[0]);
        }
        std::vector<std::string> params = Util::map<std::vector<std::string>>(param_tokens, Token::get_text);

        Stmt::u_ptr body = this->_parse_block();

        return std::make_unique<Stmt::FunDeclaration>(identifier, params, std::move(body));
    }

    Stmt::u_ptr Parser::_parse_statement()
    {
        // TODO: Consider mapping this to somekind of datastructure

        if (this->_match(TokenType::CURLY_OPEN)) {
            return this->_parse_statement_block();
        }

        if (this->_match(TokenType::IF)) {
            return this->_parse_statement_if();
        }

        if (this->_match(TokenType::WHILE)) {
            return this->_parse_statement_while();
        }

        if (this->_match(TokenType::FOR)) {
            return this->_parse_statement_for();
        }

        return this->_parse_statement_expression();
    }

    Stmt::u_ptr Parser::_parse_statement_block()
    {
        Stmt::stream statements;

        while (!this->_check(TokenType::CURLY_CLOSE) && !this->_is_at_end()) {
            statements.push_back(this->_parse_declaration());
        }

        this->_assert_next_token_is(TokenType::CURLY_CLOSE, PARSER_ERROR::UNCLOSED_BLOCK);

        return std::make_unique<Stmt::Block>(std::move(statements));
    }

    Stmt::u_ptr Parser::_parse_statement_if()
    {
        // if (expr)
        Expr::u_ptr condition = this->_parse_condition(PARSER_ERROR::IF::NO_OPEN, PARSER_ERROR::IF::NO_CLOSE);

        // { true_branch }
        Stmt::u_ptr true_branch = this->_parse_block();

        // else { false_branch }
        std::optional<Stmt::u_ptr> false_branch;
        if (this->_match(TokenType::ELSE)) {
            false_branch = this->_parse_block();
        }

        return std::make_unique<Stmt::If> (
            std::move(condition),
            std::move(true_branch), std::move(false_branch)
        );
    }

    Stmt::u_ptr Parser::_parse_statement_while()
    {
        Expr::u_ptr condition = this->_parse_condition(PARSER_ERROR::WHILE::NO_OPEN, PARSER_ERROR::WHILE::NO_CLOSE);
        Stmt::u_ptr body      = this->_parse_block();

        return std::make_unique<Stmt::While>(std::move(condition), std::move(body));
    }

    Stmt::u_ptr Parser::_parse_statement_for()
    {
        this->_assert_next_token_is(TokenType::PAREN_OPEN, PARSER_ERROR::FOR::NO_OPEN);

        std::optional<Stmt::u_ptr> init = this->_parse_for_initializer();
        Expr::u_ptr cond                = this->_parse_for_condition();
        std::optional<Expr::u_ptr> inc  = this->_parse_for_increment();
        Stmt::u_ptr body                = this->_parse_block();

        return std::make_unique<Stmt::For> (std::move(init), std::move(cond), std::move(inc), std::move(body));
    }

    Stmt::u_ptr Parser::_parse_statement_expression()
    {
        Expr::u_ptr expression = this->_parse_expression();

        this->_assert_next_token_is(TokenType::SEMICOLON, PARSER_ERROR::UNTERMINATED_EXPR);

        return std::make_unique<Stmt::Expression>(std::move(expression));
    }

    Expr::u_ptr Parser::_parse_expression()
    {
        return this->_parse_assignment();
    }

    Expr::u_ptr Parser::_parse_assignment()
    {
        if (!this->_peek(TokenType::ASSIGNMENT)) {
            // This means we fall through to logic_or
            return this->_parse_logic_or();
        }

        this->_assert_next_token_is(TokenType::IDENTIFIER, PARSER_ERROR::INVALID_ASSIGNMENT_TARGET);
        Token identifier = this->_get_previous();

        this->_advance();

        Expr::u_ptr new_value = this->_parse_logic_or();
        return std::make_unique<Expr::VarAssign>(identifier, std::move(new_value));
    }

    Expr::u_ptr Parser::_parse_logic_or()
    {
        Expr::u_ptr left = this->_parse_logic_and();

        while (this->_match(TokenType::OR)) {
            left = std::make_unique<Expr::LogicalOr>(std::move(left), this->_parse_logic_and());
        }

        return left;
    }

    Expr::u_ptr Parser::_parse_logic_and()
    {
        Expr::u_ptr left = this->_parse_equality();

        while (this->_match(TokenType::AND)) {
            left = std::make_unique<Expr::LogicalAnd>(std::move(left), this->_parse_equality());
        }

        return left;
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
            return this->_parse_call_tree();
        }

        Token op = this->_get_previous();
        return std::make_unique<Expr::Unary>(op, this->_parse_unary());
    };

    // First ( is parsed
    std::vector<Expr::u_ptr> Parser::_parse_arguments()
    {
        // Special case: no args
        if (this->_match(TokenType::PAREN_CLOSE)) {
            return {};
        }

        std::vector<Expr::u_ptr> args;
        do {
            args.push_back(this->_parse_expression());
        } while (this->_match(TokenType::COMMA));

        this->_assert_next_token_is(TokenType::PAREN_CLOSE, PARSER_ERROR::ARGS::NO_CLOSE);
        return args;
    }

    // First ( is parsed
    std::vector<Token> Parser::_parse_parameters()
    {
        if (this->_match(TokenType::PAREN_CLOSE)) {
            return {};
        }

        std::vector<Token> params;
        do {
            const Token& param = this->_assert_match(TokenType::IDENTIFIER, PARSER_ERROR::PARAMS::NOT_IDEN);
            params.push_back(param);
        } while(this->_match(TokenType::COMMA));

        this->_assert_next_token_is(TokenType::PAREN_CLOSE, PARSER_ERROR::PARAMS::NO_CLOSE);
        return params;
    }

    Expr::u_ptr Parser::_parse_call_tree()
    {
        Expr::u_ptr left = this->_parse_primary();

        while (this->_match(TokenType::PAREN_OPEN)) {
            std::vector<Expr::u_ptr> args = this->_parse_arguments();
            if (args.size() > this->_MAX_CALL_ARGS) {
                this->_panic(PARSER_ERROR::ARGS::TOO_MANY);
            }
            left = std::make_unique<Expr::Call>(std::move(left), std::move(args));
        }

        return left;
    }

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

    bool Parser::_check(TokenType type)
    {
        return this->_internal_peek(0, type);
    }

    bool Parser::_peek(TokenType type)
    {
        return this->_internal_peek(1, type);
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

    bool Parser::_internal_peek(size_t steps, TokenType type)
    {
        return this->_internal_peek(steps, [&type] (const Token& peeked) {
            return peeked.type == type;
        });
    }

    template<typename Callable>
    bool Parser::_internal_peek(size_t steps, Callable comparator)
    {
        TokenStream::const_iterator target_it = std::next(this->_current_token, steps);
        if (target_it >= this->_tokens.end()) {
            return false;
        }

        return comparator(*target_it);
    }

    const Token& Parser::_get_current()
    {
        return *this->_current_token;
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

    const Token& Parser::_assert_match(TokenType type, const char* message)
    {
        this->_assert_next_token_is(type, message);
        return this->_get_previous();
    }

    Stmt::u_ptr Parser::_parse_block()
    {
        this->_assert_next_token_is(TokenType::CURLY_OPEN, PARSER_ERROR::EXPECTED_BLOCK);

        return this->_parse_statement_block();
    }

    Expr::u_ptr Parser::_parse_condition(const char* paren_open_msg, const char* paren_close_msg)
    {
        this->_assert_next_token_is(TokenType::PAREN_OPEN, paren_open_msg);
        Expr::u_ptr condition = this->_parse_expression();
        this->_assert_next_token_is(TokenType::PAREN_CLOSE, paren_close_msg);

        return condition;
    }

    std::optional<Stmt::u_ptr> Parser::_parse_for_initializer()
    {
        if (this->_match(TokenType::SEMICOLON)) {
            return std::nullopt;
        }

        if (this->_match(TokenType::VAR)) {
            return this->_parse_declaration_vardeclaration();
        }

        return this->_parse_statement_expression();
    }

    Expr::u_ptr Parser::_parse_for_condition()
    {
        if (this->_check(TokenType::SEMICOLON)) {
            this->_panic(PARSER_ERROR::FOR::NO_CONDITION);
        }

        Expr::u_ptr expression = this->_parse_expression();
        this->_assert_next_token_is(TokenType::SEMICOLON, PARSER_ERROR::FOR::UNTERMINATED_CONDITION);
        return expression;
    }

    std::optional<Expr::u_ptr> Parser::_parse_for_increment()
    {
        std::optional<Expr::u_ptr> opt_inc;
        if (!this->_check(TokenType::PAREN_CLOSE)) {
            opt_inc = this->_parse_expression();
        }

        this->_assert_next_token_is(TokenType::PAREN_CLOSE, PARSER_ERROR::FOR::NO_CLOSE);
        return opt_inc;
    }
}