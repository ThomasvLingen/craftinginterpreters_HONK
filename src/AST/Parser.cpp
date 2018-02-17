//
// Created by mafn on 2/17/18.
//

#include "Parser.hpp"

#include "Interpreter.hpp"
#include "AST/Expr.hpp"
#include "AST/PrettyPrinter.hpp"

namespace Honk
{
    Parser::Parser(const Interpreter& parent, const TokenStream& input)
        : _parent(parent)
        , _tokens(input)
    {
    }

    void Parser::parse_input()
    {
        // Reset parser
        this->_current_token = this->_tokens.begin();

        // TODO: Remove this debug shit
        // TODO: Rethink if using unique_ptr for this is really a good idea
        // This while make_unique shebang makes me cry a little bit.
        Expr::u_ptr expression = std::make_unique<Binary>(
            std::make_unique<Unary>(
                Token {TokenType::MINUS, "-"},
                std::make_unique<Literal>(123)
            ),
            Token {TokenType::STAR, "*"},
            std::make_unique<Grouped>(
                std::make_unique<Literal>(456)
            )
        );

        PrettyASTPrinter().print(*expression);
    }
}