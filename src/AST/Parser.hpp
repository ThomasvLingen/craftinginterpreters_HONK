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

    namespace PARSER_ERROR
    {
        constexpr char BROKEN_EXPR[] = "An expression is broken (unrecognised symbols)";
        constexpr char UNCLOSED_GROUP[] = "An opening parenthesis '(' was not closed";
        constexpr char UNTERMINATED_PRINT[] = "Expected a ';' after the operand of print";
        constexpr char UNTERMINATED_EXPR[] = "Expected a ';' after the expression";
        constexpr char NO_IDENTIFIER_AFTER_VAR[] = "Expected an identifier after the 'var' keyword";
        constexpr char UNTERMINATED_VAR[] = "Expected a ';' after the variable declaration";
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
        const Interpreter& _parent;
        const TokenStream& _tokens;

        bool _has_error = false;

        TokenStream::const_iterator _current_token = _tokens.begin();
        bool _is_at_end();

        // Program grammar:
        //     program     → declaration* EOF ;
        //
        //     declaration → varDecl
        //                   | statement ;
        //
        //     varDecl     → "var" IDENTIFIER ( "=" expression )? ";" ;
        //     statement   → exprStmt
        //                   | printStmt ;
        //
        //     exprStmt    → expression ";" ;
        //     printStmt   → "print" expression ";" ; // TODO: I want to change this to "print" "(" expression ")" ";"

        Stmt::u_ptr _parse_declaration();
        Stmt::u_ptr _parse_declaration_vardeclaration();
        Stmt::u_ptr _parse_statement();
        Stmt::u_ptr _parse_statement_print();
        Stmt::u_ptr _parse_statement_expression();

        // Expression grammar:
        //     expression     → equality ;
        //     equality       → comparison ( ( "!=" | "==" ) comparison )* ;
        //     comparison     → addition ( ( ">" | ">=" | "<" | "<=" ) addition )* ;
        //     addition       → multiplication ( ( "-" | "+" ) multiplication )* ;
        //     multiplication → unary ( ( "/" | "*" ) unary )* ;
        //     unary          → ( "!" | "-" ) unary
        //                      | primary ;
        //     primary        → INT | STRING | BOOL | "null"
        //                      | IDENTIFIER
        //                      | "(" expression ")" ;
        Expr::u_ptr _parse_expression();
        Expr::u_ptr _parse_equality();
        Expr::u_ptr _parse_comparison();
        Expr::u_ptr _parse_addition();
        Expr::u_ptr _parse_multiplication();
        Expr::u_ptr _parse_unary();
        Expr::u_ptr _parse_primary();

        // this->_is_at_end() is not interwoven with this.
        // It is in the book, but it clutters everything imo.
        const Token& _advance();
        const Token& _get_current();
        const Token& _get_previous();

        bool _match(std::initializer_list<TokenType> types);
        bool _match(TokenType type);
        template <typename Callable>
        bool _match(Callable comparator);

        void _panic(const char* message);
        void _panic(const char* message, const Token& token);

        void _synchronise();
    };
}

#endif //HONK_PARSER_HPP
