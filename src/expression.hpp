#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include<string>


class Scope;
class LiteralExpression;
class VariableExpression;
class NumericDeclaration;

class ExpressionVisitor 
{
public:
    virtual void visit(const LiteralExpression& expr) = 0;
    virtual void visit(const VariableExpression& expr) = 0;
    virtual ~ExpressionVisitor() = default;
};

class Expression 
{
public:
    virtual ~Expression() = default;
    virtual void accept(ExpressionVisitor& visitor) const = 0;
};

class LiteralExpression : public Expression 
{
public:
    const std::string value;
    explicit LiteralExpression(std::string v) : value(std::move(v)) {}
    void accept(ExpressionVisitor& visitor) const override;
};

class VariableExpression : public Expression 
{
public:
    const std::string name;
    explicit VariableExpression(std::string n) : name(std::move(n)) {}
    void accept(ExpressionVisitor& visitor) const override;
};

class ExpressionEvaluator : public ExpressionVisitor 
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