#ifndef DECL_BASE_HPP
#define DECL_BASE_HPP

#include<string>
#include<vector>

#include"visitors.hpp"
#include"parser.hpp"

class Scope;
enum class ImplementedType;

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
    virtual std::string getType() const = 0;
};

class FunctionDeclaration : public Declaration
{
private:
    std::size_t num_args;
    std::vector<std::string> arg_names;
    ImplementedType return_type;
    
public:
    std::vector<ASTNode*> body_nodes;
    std::vector<DeclarationNode*> arg_nodes;
    std::shared_ptr<Scope> declaration_scope;

public:
    void accept(DeclarationVisitor& visitor) const override { visitor.visit(*this); }

    std::vector<std::string> getArgNames() const { return arg_names; }
    std::size_t getNumArgs() const { return num_args; }
    ImplementedType getReturnType() const { return return_type; }

    explicit FunctionDeclaration(   std::string_view _name, 
                                    std::size_t _num_args,
                                    std::vector<std::string> _arg_names,
                                    ImplementedType _return_type,
                                    std::vector<ASTNode*> _body_nodes,
                                    std::vector<DeclarationNode*> _args,
                                    std::shared_ptr<Scope> _declaration_scope
                                ) : Declaration(_name), 
                                    num_args(_num_args), 
                                    arg_names(_arg_names),
                                    return_type(_return_type),
                                    body_nodes( _body_nodes ),
                                    arg_nodes(_args),
                                    declaration_scope(std::move(_declaration_scope)) {}
};

#endif