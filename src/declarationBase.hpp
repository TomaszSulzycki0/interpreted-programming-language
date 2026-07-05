#ifndef DECL_BASE_HPP
#define DECL_BASE_HPP

#include<string>

class DeclarationVisitor;
class NumericDeclaration;

class Declaration
{
private:
    std::string name;
protected:
    Declaration(std::string _name) : name(std::move(_name)) {}
public:
    std::string getName() const { return name; }
    virtual void accept(DeclarationVisitor& visitor) const = 0;
    virtual ~Declaration() = default;
};

class DeclarationVisitor {
public:
    virtual void visit(const NumericDeclaration& num_decl) = 0;
    // Future expansion: virtual void visit(const StringDeclaration& strDecl) = 0;

    virtual ~DeclarationVisitor() = default;
};

class NumericDeclaration : public Declaration
{
public:
    using Declaration::Declaration;
    virtual bool isFloatingPoint() const = 0;
    virtual double asDouble() const = 0;
    virtual long long asInteger() const = 0;

    void accept(DeclarationVisitor& visitor) const override 
    {
        visitor.visit(*this);
    }

};

#endif