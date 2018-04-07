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
