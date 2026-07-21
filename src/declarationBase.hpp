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
    std::size_t num_args;
    std::vector<std::string> arg_names;
    
public:
    std::shared_ptr<Scope> scope;
    std::vector<ASTNode*> body_nodes;
    Expression* return_expr;

    virtual RuntimeValue call(const std::vector<RuntimeValue> _args)
    {

    }

    void accept(DeclarationVisitor& visitor) const override { visitor.visit(*this); }

    std::vector<std::string> getArgNames() const { return arg_names; }
    std::size_t getNumArgs() const { return num_args; }
    explicit FunctionDeclaration(   std::string_view _name, 
                                    std::size_t _num_args,
                                    std::shared_ptr<Scope> _scope,
                                    std::vector<ASTNode*> _body_nodes,
                                    Expression* _return_expr,
                                    std::vector<std::string> _arg_names
                                ) : Declaration(_name), 
                                    num_args(_num_args), 
                                    scope( std::move( _scope ) ),
                                    body_nodes( _body_nodes ),
                                    return_expr(_return_expr),
                                    arg_names(_arg_names)
    {
        
    }
};

#endif