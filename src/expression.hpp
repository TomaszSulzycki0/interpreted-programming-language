#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include<string>
#include<memory>

class ExpressionVisitor;

class Expression 
{
public:
    virtual ~Expression() = default;
    virtual void accept(ExpressionVisitor& visitor) const = 0;
};

class BinaryExpression final : public Expression
{
public:
    const std::string expr_operator;
    const std::unique_ptr<Expression> expr_left;
    const std::unique_ptr<Expression> expr_right;
    explicit BinaryExpression(std::string _operator, std::unique_ptr<Expression> _left, std::unique_ptr<Expression> _right)
        : expr_operator(std::move(_operator)), expr_left(std::move(_left)), expr_right(std::move(_right)) {}
    void accept(ExpressionVisitor& visitor) const override;
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
}
;
class UnaryExpression final : public Expression 
{
public:
    const std::string unary_op;
    const std::unique_ptr<Expression> child;
    explicit UnaryExpression(std::string op, std::unique_ptr<Expression> _child) : unary_op(std::move(op)), child(std::move(_child)) {}
    void accept(ExpressionVisitor& visitor) const override;
};

#endif