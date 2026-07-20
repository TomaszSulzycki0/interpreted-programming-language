#ifndef DECL_BASE_HPP
#define DECL_BASE_HPP

#include<string>
#include<vector>

#include"visitors.hpp"
#include"parser.hpp"

class Scope;

class Declaration
{
private:
    std::string_view name;
protected:
    explicit Declaration(std::string_view _name) : name(_name) {}
    explicit Declaration(std::string&&) = delete; 
public:
    std::string_view getName() const { return name; }
    virtual void accept(DeclarationVisitor& visitor) const = 0;
    virtual ~Declaration() = default;
};

class ValueDeclaration : public Declaration 
{
public:
    using Declaration::Declaration;
    virtual RuntimeValue getValue() const = 0;
    virtual void setValue(const RuntimeValue& val) = 0;
};

class FunctionDeclaration : public Declaration
{
    const std::size_t num_args;
    std::shared_ptr<Scope> scope;
    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    std::unique_ptr<Expression> return_expr;
public:
    explicit FunctionDeclaration(   std::string_view _name, 
                                    std::shared_ptr<Scope> _scope,
                                    std::size_t _num_args,
                                    std::vector<std::unique_ptr<ASTNode>> _body_nodes
                                ) : Declaration(_name), 
                                    num_args(_num_args), 
                                    scope( std::move( _scope ) ),
                                    body_nodes( std::move( _body_nodes ) ) 
    {
        
    }

    virtual RuntimeValue call(const std::vector<RuntimeValue> _args)
    {

    }

    void accept(DeclarationVisitor& visitor) const override { visitor.visit(*this); }
};

#endif