//
// Created by mafn on 6/17/18.
//

#ifndef HONK_RESOLVER_HPP
#define HONK_RESOLVER_HPP

#include <unordered_map>
#include <map>
#include <stack>

#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"

#include "Util/ScopeGuard.hpp"

namespace Honk
{
    // Forward declarations
    struct Interpreter;

    struct ResolvedLookup
    {
        size_t indirections;

        // Debug info
        std::string identifier;
        size_t line;
    };

    // Check how to store this. Adress or hash?
    using VariableResolveMapping = std::map<const Expr*, ResolvedLookup>; // Expression -> ResolvedLookup
    using LocalScope = std::unordered_map<std::string, bool>;         // Local Identifier -> Is initialised?

    struct ResolveError : std::runtime_error
    {
        ResolveError(const char* msg, const Token* error_token);

        const Token* error_token = nullptr;
    };




    struct Resolver
        : StmtVisitor<void>, ExprVisitor<void>
        , Util::I_Scopable<>
    {
        enum struct FunctionType
        {
            NONE,
            FUNCTION
        };

        struct CurrentFnContext : Util::I_Scopable<FunctionType>
        {
            CurrentFnContext(FunctionType& target);

            FunctionType& target;
            FunctionType old_value = FunctionType::NONE;

            void scope_enter(Resolver::FunctionType context_type) override;
            void scope_exit();
        };

        Resolver(const Interpreter& parent);

        std::optional<VariableResolveMapping> resolve(Stmt::stream& code);

        void scope_enter() override;
        void scope_exit() override;

        // Stmt visitor implementations
        void visit_Expression(Stmt::Expression& stmt) override;
        void visit_Block(Stmt::Block& stmt) override;
        void visit_If(Stmt::If& stmt) override;
        void visit_VarDeclaration(Stmt::VarDeclaration& stmt) override;
        void visit_While(Stmt::While& stmt) override;
        void visit_For(Stmt::For& stmt) override;
        void visit_FunDeclaration(Stmt::FunDeclaration& stmt) override;
        void visit_Return(Stmt::Return& stmt) override;
        void visit_Class(Stmt::Class& stmt) override;

        // Expr visitor implementations
        void visit_Binary(Expr::Binary& expr) override;
        void visit_Grouped(Expr::Grouped& expr) override;
        void visit_Literal(Expr::Literal&) override;
        void visit_LogicalOr(Expr::LogicalOr& expr) override;
        void visit_LogicalAnd(Expr::LogicalAnd& expr) override;
        void visit_Unary(Expr::Unary& expr) override;
        void visit_VarAccess(Expr::VarAccess& expr) override;
        void visit_VarAssign(Expr::VarAssign& expr) override;
        void visit_Call(Expr::Call& stmt) override;
        void visit_Fun(Expr::Fun& expr) override;
        void visit_Get(Expr::Get& expr) override;

    private:
        const Interpreter& _parent;

        VariableResolveMapping _resolved;
        std::vector<LocalScope> _scopes;

        FunctionType _current_fn = FunctionType::NONE;

        LocalScope& _get_current_scope();

        void _add_resolved_lookup(Expr* lookup, ResolvedLookup resolved);

        void _resolve(Stmt::stream& stmts);
        template <typename T>
        void _resolve(T& resolvable);
        template <typename T>
        void _opt_resolve(std::optional<typename T::u_ptr>& opt_resolvable);

        void _resolve_local(Expr& expr, const std::string& identifier);
        void _resolve_function(Expr::Fun& stmt);

        void _declare(const std::string& identifier, const Token& diagnostics_tok);
        void _define(const std::string& identifier);
        bool _is_declared_in_current_scope(const std::string& identifier);
    };
}


#endif //HONK_RESOLVER_HPP
