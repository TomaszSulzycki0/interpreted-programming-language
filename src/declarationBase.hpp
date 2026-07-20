#ifndef DECL_BASE_HPP
#define DECL_BASE_HPP

#include<string>
#include<vector>

#include"visitors.hpp"

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
    std::vector<RuntimeValue> args;
    const std::size_t num_arg;
public:
    using Declaration::Declaration;
    virtual RuntimeValue call(const std::vector<RuntimeValue> _args) = 0;
};

#endif