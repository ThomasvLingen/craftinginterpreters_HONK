//
// Created by mafn on 2/17/18.
//

#include "Parser.hpp"

#include "Interpreter.hpp"
#include "Util/Util.hpp"
#include "AST/Expr.hpp"

namespace Honk
{
    namespace ERROR
    {
        namespace ASSIGNMENT
        {
            constexpr char INVALID_TARGET[] = "Expected an identifier to the left of the '='";
        }
        namespace GROUP
        {
            constexpr char UNCLOSED[] = "An opening parenthesis '(' was not closed";
        }
        namespace VAR
        {
            constexpr char NO_IDENTIFIER[] = "Expected an identifier after the 'var' keyword";
            constexpr char UNTERMINATED[] = "Expected a ';' after the variable declaration";
        }
        namespace BLOCK
        {
            constexpr char EXPECTED[] = "Expected a code block '{ ... }'";
            constexpr char UNCLOSED[] = "The start of a code block '{' was not closed";
        }
        namespace EXPR
        {
            constexpr char BROKEN[] = "An expression is broken (unrecognised symbols)";
            constexpr char UNTERMINATED[] = "Expected a ';' after the expression";
        }
        namespace IF
        {
            constexpr char NO_OPEN[] = "Expected a '(' after the 'if' keyword";
            constexpr char NO_CLOSE[] = "Expected a ')' after the 'if' condition";
        }
        namespace WHILE
        {
            constexpr char NO_OPEN[] = "Expected a '(' after the 'while' keyword";
            constexpr char NO_CLOSE[] = "Expected a ')' after the 'while' condition";
        }
        namespace FOR
        {
            constexpr char NO_OPEN[] = "Expected a '(' after the 'for' keyword";
            constexpr char NO_CLOSE[] = "Expected a ')' after the 'for' clauses";
            constexpr char NO_CONDITION[] = "Expected an expression as the second clause for a 'for' loop";
            constexpr char UNTERMINATED_CONDITION[] = "Expected a ';' after the for loop's condition";
        }
        namespace FUN
        {
            constexpr char NO_IDENTIFIER[] = "Expected identifier in function declaration";
            constexpr char NO_PARAMS[] = "Expected a '(' (parameter declarations) after the function identifier";
            constexpr char NO_BODY[] = "Expected a '{' (function body) after a parameter declaration";
        }
        namespace RETURN
        {
            constexpr char UNTERMINATED[] = "Expected a ';' after the return statement";
        }
        namespace ARGS
        {
            constexpr char NO_CLOSE[] = "Expected a ')' after a list of arguments";
            constexpr char TOO_MANY[] = "Too many arguments (max 8)";
        }
        namespace PARAMS
        {
            constexpr char NOT_IDEN[] = "Expected an identifier in param list";
            constexpr char NO_CLOSE[] = "Expected a ')' after a list of parameters";
            constexpr char TOO_MANY[] = "Too many parameters (max 8)";
        }
    }

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
        Token identifier = this->_assert_match(TokenType::IDENTIFIER, ERROR::VAR::NO_IDENTIFIER);

        std::optional<Expr::u_ptr> initializer;
        if (this->_match(TokenType::ASSIGNMENT)) {
            initializer = std::move(this->_parse_expression());
        }

        this->_assert_match(TokenType::SEMICOLON, ERROR::VAR::UNTERMINATED);
        return std::make_unique<Stmt::VarDeclaration>(identifier, std::move(initializer));
    }

    // fun keyword is parsed
    Stmt::u_ptr Parser::_parse_declaration_fun()
    {
        Token identifier = this->_assert_match(TokenType::IDENTIFIER, ERROR::FUN::NO_IDENTIFIER);

        this->_assert_match(TokenType::PAREN_OPEN, ERROR::FUN::NO_PARAMS);
        std::vector<Token> param_tokens = this->_parse_parameters();
        if (param_tokens.size() > this->_MAX_CALL_ARGS) {
            this->_panic(ERROR::PARAMS::TOO_MANY, param_tokens[0]);
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

        if (this->_match(TokenType::RETURN)) {
            return this->_parse_statement_return();
        }

        return this->_parse_statement_expression();
    }

    Stmt::u_ptr Parser::_parse_statement_block()
    {
        Stmt::stream statements;

        while (!this->_check(TokenType::CURLY_CLOSE) && !this->_is_at_end()) {
            statements.push_back(this->_parse_declaration());
        }

        this->_assert_match(TokenType::CURLY_CLOSE, ERROR::BLOCK::UNCLOSED);

        return std::make_unique<Stmt::Block>(std::move(statements));
    }

    Stmt::u_ptr Parser::_parse_statement_if()
    {
        // if (expr)
        Expr::u_ptr condition = this->_parse_condition(ERROR::IF::NO_OPEN, ERROR::IF::NO_CLOSE);

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
        Expr::u_ptr condition = this->_parse_condition(ERROR::WHILE::NO_OPEN, ERROR::WHILE::NO_CLOSE);
        Stmt::u_ptr body      = this->_parse_block();

        return std::make_unique<Stmt::While>(std::move(condition), std::move(body));
    }

    Stmt::u_ptr Parser::_parse_statement_for()
    {
        this->_assert_match(TokenType::PAREN_OPEN, ERROR::FOR::NO_OPEN);

        std::optional<Stmt::u_ptr> init = this->_parse_for_initializer();
        Expr::u_ptr cond                = this->_parse_for_condition();
        std::optional<Expr::u_ptr> inc  = this->_parse_for_increment();
        Stmt::u_ptr body                = this->_parse_block();

        return std::make_unique<Stmt::For> (std::move(init), std::move(cond), std::move(inc), std::move(body));
    }

    Stmt::u_ptr Parser::_parse_statement_expression()
    {
        Expr::u_ptr expression = this->_parse_expression();

        this->_assert_match(TokenType::SEMICOLON, ERROR::EXPR::UNTERMINATED);

        return std::make_unique<Stmt::Expression>(std::move(expression));
    }

    Stmt::u_ptr Parser::_parse_statement_return()
    {
        Token keyword = this->_get_previous();
        Expr::u_ptr return_value;

        if (this->_check(TokenType::SEMICOLON)) {
            return_value = std::make_unique<Expr::Literal>(null);
        } else {
            return_value = this->_parse_expression();
        }

        this->_assert_match(TokenType::SEMICOLON, ERROR::RETURN::UNTERMINATED);
        return std::make_unique<Stmt::Return>(keyword, std::move(return_value));
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

        Token identifier = this->_assert_match(TokenType::IDENTIFIER, ERROR::ASSIGNMENT::INVALID_TARGET);

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

        while (Token::opt op = this->_match(_match_equality)) {
            Expr::u_ptr right = this->_parse_comparison();
            left = std::make_unique<Expr::Binary>(std::move(left), *op, std::move(right));
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

        while (Token::opt op = this->_match(_match_comparison)) {
            Expr::u_ptr right = this->_parse_addition();
            left = std::make_unique<Expr::Binary>(std::move(left), *op, std::move(right));
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

        while (Token::opt op = this->_match(_match_addition)) {
            Expr::u_ptr right = this->_parse_multiplication();
            left = std::make_unique<Expr::Binary>(std::move(left), *op, std::move(right));
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

        while (Token::opt op = this->_match(_match_multiplication)) {
            Expr::u_ptr right = this->_parse_unary();
            left = std::make_unique<Expr::Binary>(std::move(left), *op, std::move(right));
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
        if (Token::opt op = this->_match(_match_unary)) {
            return std::make_unique<Expr::Unary>(*op, this->_parse_unary());
        }

        return this->_parse_call_tree();
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

        this->_assert_match(TokenType::PAREN_CLOSE, ERROR::ARGS::NO_CLOSE);
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
            const Token& param = this->_assert_match(TokenType::IDENTIFIER, ERROR::PARAMS::NOT_IDEN);
            params.push_back(param);
        } while(this->_match(TokenType::COMMA));

        this->_assert_match(TokenType::PAREN_CLOSE, ERROR::PARAMS::NO_CLOSE);
        return params;
    }

    Expr::u_ptr Parser::_parse_call_tree()
    {
        Expr::u_ptr left = this->_parse_primary();

        while (this->_match(TokenType::PAREN_OPEN)) {
            std::vector<Expr::u_ptr> args = this->_parse_arguments();
            if (args.size() > this->_MAX_CALL_ARGS) {
                this->_panic(ERROR::ARGS::TOO_MANY);
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
        if (Token::opt literal = this->_match(_match_literal)) {
            return std::make_unique<Expr::Literal>(literal->value);
        }

        if (Token::opt identifier = this->_match(TokenType::IDENTIFIER)) {
            return std::make_unique<Expr::VarAccess>(*identifier);
        }

        if (this->_match(TokenType::PAREN_OPEN)) {
            Expr::u_ptr grouped_expr = this->_parse_expression();
            this->_assert_match(TokenType::PAREN_CLOSE, ERROR::GROUP::UNCLOSED);

            return std::make_unique<Expr::Grouped>(std::move(grouped_expr));
        }

        this->_panic(ERROR::EXPR::BROKEN);
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

    auto type_comparator(TokenType type)
    {
        return [type] (const Token& token) {
            return token.type == type;
        };
    }

    bool Parser::_check(TokenType type)
    {
        return this->_check_source(0, type_comparator(type));
    }

    bool Parser::_peek(TokenType type)
    {
        return this->_check_source(1, type_comparator(type));
    }

    Token::opt Parser::_match(TokenType type)
    {
        return this->_match(type_comparator(type));
    }

    template <typename Callable>
    Token::opt Parser::_match(Callable comparator)
    {
        if (!this->_check_source(0, comparator)) {
            return std::nullopt;
        }

        return this->_advance();
    }

    Token Parser::_assert_match(TokenType type, const char* message)
    {
        if (Token::opt token = this->_match(type)) {
            return *token;
        }

        this->_panic(message);
    }

    template<typename Callable>
    TokenStream::const_iterator Parser::_inspect_source(size_t steps, Callable comparator)
    {
        TokenStream::const_iterator target_it = std::next(this->_current_token, steps);
        if (target_it < this->_tokens.end()) {
            if (comparator(*target_it)) {
                return target_it;
            }
        }

        return this->_tokens.end();
    }

    template<typename Callable>
    bool Parser::_check_source(size_t steps, Callable comparator)
    {
        return this->_inspect_source(steps, comparator) != this->_tokens.end();
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

    Stmt::u_ptr Parser::_parse_block()
    {
        this->_assert_match(TokenType::CURLY_OPEN, ERROR::BLOCK::EXPECTED);

        return this->_parse_statement_block();
    }

    Expr::u_ptr Parser::_parse_condition(const char* paren_open_msg, const char* paren_close_msg)
    {
        this->_assert_match(TokenType::PAREN_OPEN, paren_open_msg);
        Expr::u_ptr condition = this->_parse_expression();
        this->_assert_match(TokenType::PAREN_CLOSE, paren_close_msg);

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
            this->_panic(ERROR::FOR::NO_CONDITION);
        }

        Expr::u_ptr expression = this->_parse_expression();
        this->_assert_match(TokenType::SEMICOLON, ERROR::FOR::UNTERMINATED_CONDITION);
        return expression;
    }

    std::optional<Expr::u_ptr> Parser::_parse_for_increment()
    {
        std::optional<Expr::u_ptr> opt_inc;
        if (!this->_check(TokenType::PAREN_CLOSE)) {
            opt_inc = this->_parse_expression();
        }

        this->_assert_match(TokenType::PAREN_CLOSE, ERROR::FOR::NO_CLOSE);
        return opt_inc;
    }
}