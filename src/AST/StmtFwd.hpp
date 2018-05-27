//
// Created by mafn on 5/27/18.
//

#ifndef HONK_STMTFWD_HPP
#define HONK_STMTFWD_HPP

#include <string>
#include <memory>

namespace Honk
{
    template<typename T>
    struct StmtVisitor;

    struct Token;

    struct Stmt
    {
        virtual ~Stmt() = default;

        virtual void accept(StmtVisitor<void>& visitor) = 0;
        virtual std::string accept(StmtVisitor<std::string>& visitor) = 0;

        const Token& diagnostics_token;

        using u_ptr = std::unique_ptr<Stmt>;
        using stream = std::vector<u_ptr>;

        struct Expression;
        struct Block;
        struct VarDeclaration;
        struct If;
        struct While;
        struct For;
        struct FunDeclaration;

    private:
        Stmt();
        Stmt(const Token& diagnostics_token);
    };
}

#endif //HONK_STMTFWD_HPP
