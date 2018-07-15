//
// Created by mafn on 6/17/18.
//

#include "Resolver.hpp"

#include "Interpreter.hpp"

namespace Honk
{
    ResolveError::ResolveError(const char* msg, const Token* error_token)
        : runtime_error(msg)
        , error_token(error_token)
    {
    }

    Resolver::Resolver(const Interpreter& parent)
        : _parent(parent)
    {
    }

    VariableResolveMapping Resolver::resolve(Stmt::stream& code)
    {
        try {
            this->_resolve(code);
        } catch (ResolveError& e) {
            this->_parent.report_error(e.error_token->line, e.what());
        }

        return this->_resolved;
    }

    template<typename T>
    void Resolver::_resolve(T& resolvable)
    {
        resolvable.accept(*this);
    }

    void Resolver::_resolve(Stmt::stream& stmts)
    {
        for (Stmt::u_ptr& stmt : stmts) {
            this->_resolve(*stmt);
        }
    }

    void Resolver::scope_enter()
    {
        this->_scopes.push_back(LocalScope {});
    }

    void Resolver::scope_exit()
    {
        this->_scopes.pop_back();
    }

    void Resolver::_declare(const std::string& identifier)
    {
        if (this->_scopes.empty()) {
            return;
        }

        LocalScope& scope = this->_get_current_scope();
        scope[identifier] = false;
    }

    void Resolver::_define(const std::string& identifier)
    {
        if (this->_scopes.empty()) {
            return;
        }

        LocalScope& scope = this->_get_current_scope();
        scope[identifier] = true;
    }

    LocalScope& Resolver::_get_current_scope()
    {
        return this->_scopes.back();
    }

    bool Resolver::_is_declared_in_current_scope(const std::string& identifier)
    {
        if (this->_scopes.empty()
            || !Util::contains(this->_get_current_scope(), identifier))
        {
            return false;
        }

        return !this->_get_current_scope()[identifier];
    }

    void Resolver::_resolve_function(Expr::Fun& expr)
    {
        Util::ScopeGuard<> function_scope(*this);

        for (std::string& param : expr.parameters) {
            this->_declare(param);
            this->_define(param);
        }

        this->_resolve(expr.get_body().statements);
    }

    void Resolver::_resolve_local(Expr& expr, const std::string& identifier)
    {
        for (std::vector<LocalScope>::reverse_iterator it = this->_scopes.rbegin();
             it != this->_scopes.rend();
             it++)
        {
            if (Util::contains(*it, identifier)) {
                int indirections = abs(std::distance(it, this->_scopes.rbegin()));

                this->_add_resolved_lookup(&expr, ResolvedLookup {
                    static_cast<size_t>(indirections),
                    identifier,
                    expr.diagnostics_token.line
                });
            }
        }

        // Unresolved, assumed global
    }

    void Resolver::_add_resolved_lookup(Expr* lookup, ResolvedLookup resolved)
    {
        this->_resolved[lookup] = resolved;
    }

    void Resolver::visit_Expression(Stmt::Expression& stmt)
    {
        this->_resolve(*stmt.expression);
    }

    void Resolver::visit_Block(Stmt::Block& stmt)
    {
        Util::ScopeGuard<> scope_guard(*this);
        this->_resolve(stmt.statements);
    }

    void Resolver::visit_If(Stmt::If& stmt)
    {
        this->_resolve(*stmt.condition);
        this->_resolve(*stmt.true_branch);

        this->_opt_resolve<Stmt>(stmt.false_branch);
    }

    void Resolver::visit_VarDeclaration(Stmt::VarDeclaration& stmt)
    {
        const std::string& identifier = stmt.get_identifier();

        this->_declare(identifier);
        this->_opt_resolve<Expr>(stmt.initializer);
        this->_define(identifier);
    }

    void Resolver::visit_While(Stmt::While& stmt)
    {
        this->_resolve(*stmt.condition);
        this->_resolve(*stmt.body);
    }

    void Resolver::visit_For(Stmt::For& stmt)
    {
        Util::ScopeGuard<> scope_guard(*this); // Scope for the for clause
        this->_opt_resolve<Stmt>(stmt.initializer);
        this->_resolve(*stmt.condition);
        this->_opt_resolve<Expr>(stmt.increment);
        this->_resolve(*stmt.body);
    }

    void Resolver::visit_FunDeclaration(Stmt::FunDeclaration& stmt)
    {
        this->_declare(stmt.get_identifier());
        this->_define(stmt.get_identifier());

        this->_resolve(stmt.get_fun());
    }

    void Resolver::visit_Return(Stmt::Return& stmt)
    {
        this->_opt_resolve<Expr>(stmt.return_value);
    }

    void Resolver::visit_Binary(Expr::Binary& expr)
    {
        this->_resolve(*expr.left);
        this->_resolve(*expr.right);
    }

    void Resolver::visit_Grouped(Expr::Grouped& expr)
    {
        this->_resolve(*expr.expression);
    }

    void Resolver::visit_Literal(Expr::Literal&)
    {
        // Nothing to resolve...
    }

    void Resolver::visit_LogicalOr(Expr::LogicalOr& expr)
    {
        this->_resolve(*expr.left);
        this->_resolve(*expr.right);
    }

    void Resolver::visit_LogicalAnd(Expr::LogicalAnd& expr)
    {
        this->_resolve(*expr.left);
        this->_resolve(*expr.right);
    }

    void Resolver::visit_Unary(Expr::Unary& expr)
    {
        this->_resolve(*expr.right);
    }

    void Resolver::visit_VarAccess(Expr::VarAccess& expr)
    {
        if (_is_declared_in_current_scope(expr.get_identifier())) {
            throw ResolveError("Initialiser refers to itself!", &expr.identifier_tok);
        }

        this->_resolve_local(expr, expr.get_identifier());
    }

    void Resolver::visit_VarAssign(Expr::VarAssign& expr)
    {
        this->_resolve(*expr.new_value);
        this->_resolve_local(expr, expr.get_identifier());
    }

    void Resolver::visit_Call(Expr::Call& stmt)
    {
        this->_resolve(*stmt.callee);

        for (Expr::u_ptr& call_arg : stmt.args) {
            this->_resolve(*call_arg);
        }
    }

    void Resolver::visit_Fun(Expr::Fun& expr)
    {
        this->_resolve_function(expr);
    }

    template<typename T>
    void Resolver::_opt_resolve(std::optional<typename T::u_ptr>& opt_resolvable)
    {
        if (opt_resolvable) {
            this->_resolve(**opt_resolvable);
        }
    }
}
