#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include<string>

class ExpressionVisitor;

class Expression 
{
public:
    virtual ~Expression() = default;
    virtual void accept(ExpressionVisitor& visitor) const = 0;
};

class LiteralExpression final : public Expression 
{
public:
    const std::string value;
    explicit LiteralExpression(std::string v) : value(std::move(v)) {}
    void accept(ExpressionVisitor& visitor) const override;
};

class VariableExpression final : public Expression 
{
public:
    const std::string name;
    explicit VariableExpression(std::string n) : name(std::move(n)) {}
    void accept(ExpressionVisitor& visitor) const override;
};

#endif