//
// Created by mafn on 2/17/18.
//

#ifndef HONK_PARSER_HPP
#define HONK_PARSER_HPP

#include <vector>

#include "Lexer/Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

namespace Honk
{
    // Forward declarations
    struct Interpreter;

    struct Parser
    {
        Parser(const Interpreter& parent, const TokenStream& input);

        std::optional<Stmt::stream> parse_input();
        std::optional<Expr::u_ptr> parse_input_as_expr();

    private:
        const Interpreter& _parent;
        const TokenStream& _tokens;

        bool _has_error = false;

        TokenStream::const_iterator _current_token = _tokens.begin();
        bool _is_at_end();

        // Program grammar:
        //     program   → statement* EOF ;
        //
        //     statement → exprStmt
        //                 | printStmt ;
        //
        //     exprStmt  → expression ";" ;
        //     printStmt → "print" expression ";" ; // TODO: I want to change this to "print" "(" expression ")" ";"
        Stmt::u_ptr _parse_statement();

        // Expression grammar:
        //     expression     → equality ;
        //     equality       → comparison ( ( "!=" | "==" ) comparison )* ;
        //     comparison     → addition ( ( ">" | ">=" | "<" | "<=" ) addition )* ;
        //     addition       → multiplication ( ( "-" | "+" ) multiplication )* ;
        //     multiplication → unary ( ( "/" | "*" ) unary )* ;
        //     unary          → ( "!" | "-" ) unary
        //                      | primary ;
        //     primary        → INT | STRING | BOOL | "null"
        //                      | "(" expression ")" ;
        Expr::u_ptr _parse_expression();
        Expr::u_ptr _parse_equality();
        Expr::u_ptr _parse_comparison();
        Expr::u_ptr _parse_addition();
        Expr::u_ptr _parse_multiplication();
        Expr::u_ptr _parse_unary();
        Expr::u_ptr _parse_primary();

        // TODO: Determine whether an "isAtEnd" should be interwoven with this
        // It is in the book, but it clutters everything imo and I don't know why it's in there.
        const Token& _advance();
        const Token& _get_current();
        const Token& _get_previous();

        bool _match(std::initializer_list<TokenType> types);
        bool _match(TokenType type);
        template <typename Callable>
        bool _match(Callable comparator);

        Expr::u_ptr _error_expr();
        void _error_broken_expression();
        void _error_unclosed_param();
    };
}

#endif //HONK_PARSER_HPP
