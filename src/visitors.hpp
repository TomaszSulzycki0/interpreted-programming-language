#ifndef VISITORS_HPP
#define VISITORS_HPP

#include<iostream>

class NumericDeclaration;
class LiteralExpression;
class VariableExpression;
class Expression;
class Scope;

class DeclarationVisitor {
public:
    virtual void visit(const NumericDeclaration& num_decl) = 0;
    // Future expansion: virtual void visit(const StringDeclaration& strDecl) = 0;

    virtual ~DeclarationVisitor() = default;
};

class PrintVisitor final : public DeclarationVisitor 
{
public:
    void visit(const NumericDeclaration& num_decl) override;
};

class ExpressionVisitor 
{
public:
    virtual void visit(const LiteralExpression& expr) = 0;
    virtual void visit(const VariableExpression& expr) = 0;
    virtual ~ExpressionVisitor() = default;
};

class ExpressionEvaluator final : public ExpressionVisitor 
{
private:
    const Scope& scope;
    double last_evaluated_value = 0.0;

public:
    explicit ExpressionEvaluator(const Scope& s) : scope(s) {}
    double evaluate(const Expression& expr);
    void visit(const LiteralExpression& expr) override;
    void visit(const VariableExpression& expr) override;
};

#endif