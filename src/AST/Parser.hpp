//
// Created by mafn on 2/17/18.
//

#ifndef HONK_PARSER_HPP
#define HONK_PARSER_HPP

#include <vector>

#include "Lexer/Token.hpp"
#include "Expr.hpp"

namespace Honk
{
    // Forward declarations
    struct Interpreter;

    struct Parser
    {
        Parser(const Interpreter& parent, const TokenStream& input);

        // TODO: Make this return the parsed stuff.
        void parse_input();

    private:
        const Interpreter& _parent;
        const TokenStream& _tokens;

        TokenStream::const_iterator _current_token = _tokens.begin();
        bool _is_at_end();

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
    };
}

#endif //HONK_PARSER_HPP
