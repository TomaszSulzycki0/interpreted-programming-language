#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include<string>
#include<vector>
#include<memory>

class ExpressionVisitor;
enum class ImplementedType;

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
    
    explicit BinaryExpression(std::string _operator, std::unique_ptr<Expression> _left, std::unique_ptr<Expression> _right)
        : expr_operator(std::move(_operator)), expr_left(std::move(_left)), expr_right(std::move(_right)) {}

    BinaryExpression( const BinaryExpression& other) 
        :   expr_operator(other.expr_operator), 
            expr_left(other.expr_left ? other.expr_left->clone() : nullptr),
            expr_right(other.expr_right ? other.expr_right->clone() : nullptr) {}

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
    const ImplementedType tp;
    explicit LiteralExpression(std::string v, ImplementedType _tp) : value(std::move(v)), tp(_tp) {}
    LiteralExpression(const LiteralExpression& other) : value(other.value), tp(other.tp) {}

    void accept(ExpressionVisitor& visitor) const override;

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<LiteralExpression>(*this);
    }
};

class VariableExpression final : public Expression 
{
public:
    const std::string name;
    explicit VariableExpression(std::string n) : name(std::move(n)) {}
    VariableExpression(const VariableExpression& other) : name(other.name) {} 

    void accept(ExpressionVisitor& visitor) const override;

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<VariableExpression>(*this);
    }
};

class UnaryExpression final : public Expression 
{
public:
    const std::string unary_op;
    const std::unique_ptr<Expression> child;

    explicit UnaryExpression(std::string op, std::unique_ptr<Expression> _child) 
        : unary_op(std::move(op)), child(std::move(_child)) {}

    UnaryExpression(const UnaryExpression& other) : unary_op(other.unary_op), child( other.child ? other.child->clone() : nullptr ) {}
    void accept(ExpressionVisitor& visitor) const override;

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<UnaryExpression>(*this);
    }
};

class FunctionCallExpression final : public Expression 
{
public:
    const std::string name;
    std::vector<std::unique_ptr<Expression>> args;

    explicit FunctionCallExpression(std::string n, std::vector<std::unique_ptr<Expression>> _args) 
        : name(std::move(n)), args( std::move(_args)) {}

    FunctionCallExpression(const FunctionCallExpression& other) : name(other.name)
    {
        args.reserve( other.args.size() );

        for (const auto& arg : other.args) 
        {
            if (arg) 
            {
                args.push_back(arg->clone());
            } 
            else 
            {
                args.push_back(nullptr);
            }
        }
    }

    void accept(ExpressionVisitor& visitor) const override;

    std::unique_ptr<Expression> clone() const override 
    {
        return std::make_unique<FunctionCallExpression>(*this);
    }
};

#endif