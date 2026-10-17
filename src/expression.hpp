#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include<string>
#include<vector>
#include<memory>

#include"parser.hpp"

class ExpressionVisitor;

// Base class for all expressions
class Expression 
{
public:
    virtual ~Expression() = default;
    virtual void accept(ExpressionVisitor& visitor) const = 0;
    virtual std::unique_ptr<Expression> clone() const = 0;
};

class BinaryExpression final : public Expression
{
public:
    const std::string expr_operator;
    const std::unique_ptr<Expression> expr_left;
    const std::unique_ptr<Expression> expr_right;
    
    explicit BinaryExpression(std::string _operator, std::unique_ptr<Expression> _left, std::unique_ptr<Expression> _right);
    BinaryExpression( const BinaryExpression& other);

    void accept(ExpressionVisitor& visitor) const override;

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<BinaryExpression>(*this);
    }
};

class LiteralExpression final : public Expression 
{
public:
    const std::string value;
    const std::string tp;
    explicit LiteralExpression(std::string v, std::string _tp);
    LiteralExpression(const LiteralExpression& other);

    void accept(ExpressionVisitor& visitor) const override;

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<LiteralExpression>(*this);
    }
};


class UnaryExpression final : public Expression 
{
public:
    const std::string unary_op;
    const std::unique_ptr<Expression> child;

    explicit UnaryExpression(std::string op, std::unique_ptr<Expression> _child);
    UnaryExpression(const UnaryExpression& other);

    void accept(ExpressionVisitor& visitor) const override;

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<UnaryExpression>(*this);
    }
};

class NamedExpression : public Expression
{
public:
    const virtual std::string getName() const = 0;

};

class VariableExpression final : public NamedExpression
{
public:
    std::string name;
    explicit VariableExpression(std::string n);
    VariableExpression(const VariableExpression& other);

    void accept(ExpressionVisitor& visitor) const override;

    const std::string getName() const override 
    {
        return name;   
    }

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<VariableExpression>(*this);
    }
};

class FunctionCallExpression final : public NamedExpression
{
public:
    const std::string name;
    std::vector<std::unique_ptr<Expression>> args;

    explicit FunctionCallExpression(std::string n, std::vector<std::unique_ptr<Expression>> _args);
    FunctionCallExpression(const FunctionCallExpression& other);

    void accept(ExpressionVisitor& visitor) const override;

    const std::string getName() const override 
    {
        return name;
    }

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<FunctionCallExpression>(*this);
    }
};

class AccessMemberExpression final : public NamedExpression
{
public:

    std::unique_ptr<NamedExpression> left_side_expr;
    std::string member;

    explicit AccessMemberExpression( std::unique_ptr<NamedExpression> expr, std::string mem );
    AccessMemberExpression(const AccessMemberExpression& other);
    
    void accept(ExpressionVisitor& visitor) const override;

    const std::string getName() const override 
    {
        return member;   
    }

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<AccessMemberExpression>(*this);
    }
};

class MultiExpression final : public Expression
{
public:

    std::vector<std::unique_ptr<Expression>> expr_list;

    explicit MultiExpression(std::vector<std::unique_ptr<Expression>> expressions);

    MultiExpression(const MultiExpression& other);

    void accept(ExpressionVisitor& visitor) const override;
  
    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<MultiExpression>(*this);
    }
};

#endif