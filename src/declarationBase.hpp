#ifndef DECL_BASE_HPP
#define DECL_BASE_HPP

#include<string>

#include"visitors.hpp"

class Declaration
{
private:
    std::string name;
protected:
    explicit Declaration(std::string _name) : name(std::move(_name)) {}
public:
    std::string getName() const { return name; }
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

#endif