//
// Created by mafn on 2/17/18.
//

#ifndef HONK_PARSER_HPP
#define HONK_PARSER_HPP

#include <vector>
#include <optional>

#include "Lexer/Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

namespace Honk
{
    // Forward declarations
    struct Interpreter;

    namespace PARSER_ERROR
    {
        // TODO: refactor to be namespaced for clarity
        constexpr char BROKEN_EXPR[] = "An expression is broken (unrecognised symbols)";
        constexpr char UNCLOSED_GROUP[] = "An opening parenthesis '(' was not closed";
        constexpr char UNCLOSED_BLOCK[] = "The start of a code block '{' was not closed";
        constexpr char UNTERMINATED_EXPR[] = "Expected a ';' after the expression";
        constexpr char NO_IDENTIFIER_AFTER_VAR[] = "Expected an identifier after the 'var' keyword";
        constexpr char UNTERMINATED_VAR[] = "Expected a ';' after the variable declaration";
        constexpr char INVALID_ASSIGNMENT_TARGET[] = "Expected an identifier to the left of the '='";

        constexpr char EXPECTED_BLOCK[] = "Expected a code block '{ ... }'";

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

    struct parse_exception : std::runtime_error
    {
        parse_exception(const char* error_msg, const Token& token)
            : std::runtime_error(error_msg)
            , token(token)
        {
        }

        const Token& token;
    };

    struct Parser
    {
        Parser(const Interpreter& parent, const TokenStream& input);

        std::optional<Stmt::stream> parse_input();
        std::optional<Expr::u_ptr> parse_input_as_expr();

    private:
        static constexpr size_t _MAX_CALL_ARGS = 8;

        const Interpreter& _parent;
        const TokenStream& _tokens;

        bool _has_error = false;

        TokenStream::const_iterator _current_token = _tokens.begin();
        bool _is_at_end();

        // Program grammar:
        //     program         → declaration* EOF ;
        //
        //     declaration     → var_declaration
        //                     | fun_declaration
        //                     | statement ;
        //
        //     var_declaration → "var" IDENTIFIER ( "=" expression )? ";" ;
        //     fun_declaration → "fun" function ;
        //     function        → IDENTIFIER "(" parameters? ")" stmt_block ;
        //     parameters      → IDENTIFIER ( "," IDENTIFIER )* ;
        //     statement       → stmt_expr
        //                     | stmt_block
        //                     | stmt_if
        //                     | stmt_while
        //                     | stmt_for ;
        //
        //     stmt_expr       → expression ";" ;
        //     stmt_block      → "{" declaration* "}" ;
        //     stmt_if         → "if" "(" expression ")" stmt_block ( "else" stmt_block )? ;
        //     stmt_while      → "while" "(" expression ")" stmt_block ;
        //     stmt_for        → "for" "("
        //                                 (var_declaration | stmt_expr | ";")
        //                                 expression ";"
        //                                 expression?
        //                              ")" stmt_block ;

        Stmt::u_ptr _parse_protected_decl();
        Stmt::u_ptr _parse_declaration();
        Stmt::u_ptr _parse_declaration_vardeclaration();
        Stmt::u_ptr _parse_declaration_fun();
        Stmt::u_ptr _parse_statement();
        Stmt::u_ptr _parse_statement_expression();
        Stmt::u_ptr _parse_statement_block();
        Stmt::u_ptr _parse_statement_if();
        Stmt::u_ptr _parse_statement_while();
        Stmt::u_ptr _parse_statement_for();

        // Expression grammar:
        //     expression     → assignment ;
        //     assignment     → IDENTIFIER "=" logic_or
        //                    | logic_or ;
        //
        //     logic_or       → logic_and ( "or" logic_and )* ;
        //     logic_and      → equality ( "and" equality )* ;
        //
        //     equality       → comparison ( ( "!=" | "==" ) comparison )* ;
        //     comparison     → addition ( ( ">" | ">=" | "<" | "<=" ) addition )* ;
        //     addition       → multiplication ( ( "-" | "+" ) multiplication )* ;
        //     multiplication → unary ( ( "/" | "*" ) unary )* ;
        //     unary          → ( "!" | "-" ) unary
        //                    | call_tree ;
        //     call_tree      → primary ( "(" arguments? ")" )* ;
        //     arguments      → expression ( "," expression )* ;
        //     primary        → INT | STRING | BOOL | "null"
        //                    | IDENTIFIER
        //                    | "(" expression ")" ;
        Expr::u_ptr _parse_expression();
        Expr::u_ptr _parse_assignment();
        Expr::u_ptr _parse_logic_or();
        Expr::u_ptr _parse_logic_and();
        Expr::u_ptr _parse_equality();
        Expr::u_ptr _parse_comparison();
        Expr::u_ptr _parse_addition();
        Expr::u_ptr _parse_multiplication();
        Expr::u_ptr _parse_unary();
        Expr::u_ptr _parse_call_tree();
        Expr::u_ptr _parse_primary();

        // _parse_helpers
        Stmt::u_ptr _parse_block();
        Expr::u_ptr _parse_condition(const char* paren_open_msg, const char* paren_close_msg);
        std::optional<Stmt::u_ptr> _parse_for_initializer();
        Expr::u_ptr _parse_for_condition();
        std::optional<Expr::u_ptr> _parse_for_increment();
        std::vector<Expr::u_ptr> _parse_arguments();
        std::vector<Token> _parse_parameters();

        // this->_is_at_end() is not interwoven with this.
        // It is in the book, but it clutters everything imo.
        const Token& _advance();
        const Token& _get_current();
        const Token& _get_previous();

        // TODO: I am not happy with the amount of utility match/peek/check code I have...
        bool _match(TokenType type);
        template <typename Callable>
        bool _match(Callable comparator);

        bool _peek(TokenType type);
        bool _check(TokenType type);

        template <typename Callable>
        bool _internal_peek(size_t steps, Callable comparator);
        bool _internal_peek(size_t steps, TokenType type);

        void _panic(const char* message);
        void _panic(const char* message, const Token& token);

        void _assert_next_token_is(TokenType type, const char* message);
        const Token& _assert_match(TokenType type, const char* message);

        void _synchronise();
    };
}

#endif //HONK_PARSER_HPP
