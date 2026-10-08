#include"expression.hpp"
#include"declarationBase.hpp"
#include"scope.hpp"
#include"visitorsBase.hpp"
#include"parser.hpp"

BinaryExpression::BinaryExpression(std::string _operator, std::unique_ptr<Expression> _left, std::unique_ptr<Expression> _right)
    : expr_operator(std::move(_operator)), expr_left(std::move(_left)), expr_right(std::move(_right)) {}

BinaryExpression::BinaryExpression( const BinaryExpression& other) 
        :   expr_operator(other.expr_operator), 
            expr_left(other.expr_left ? other.expr_left->clone() : nullptr),
            expr_right(other.expr_right ? other.expr_right->clone() : nullptr) {}

LiteralExpression::LiteralExpression(std::string v, ImplementedType _tp) : value(std::move(v)), tp(_tp) {}

LiteralExpression::LiteralExpression(const LiteralExpression& other) : value(other.value), tp(other.tp) {}

VariableExpression::VariableExpression(std::unique_ptr<VariableAccess> v) : var(std::move(v)) {}

VariableExpression::VariableExpression(const VariableExpression& other) 
{
    if ( other.var )
    {
        var = std::make_unique<VariableAccess>(*other.var);
    }
    else
    {
        var = nullptr;
    }
}

UnaryExpression::UnaryExpression(std::string op, std::unique_ptr<Expression> _child) 
    : unary_op(std::move(op)), child(std::move(_child)) {}

UnaryExpression::UnaryExpression(const UnaryExpression& other) : unary_op(other.unary_op), child( other.child ? other.child->clone() : nullptr ) {}

FunctionCallExpression::FunctionCallExpression(std::string n, std::vector<std::unique_ptr<Expression>> _args) 
    : name(std::move(n)), args( std::move(_args)) {}

FunctionCallExpression::FunctionCallExpression(const FunctionCallExpression& other) : name(other.name)
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

AccessMemberExpression::AccessMemberExpression( std::unique_ptr<Expression> expr,
        std::string mem ) :
        left_side_expr(std::move(expr)), member(mem) {}

AccessMemberExpression::AccessMemberExpression(const AccessMemberExpression& other)
{
    if ( other.left_side_expr )
    {
        left_side_expr = other.left_side_expr->clone();
    }
    else
    {
        left_side_expr = nullptr;
    }
    
    member = other.member;
}

void LiteralExpression::accept(ExpressionVisitor& visitor) const 
{
    visitor.visit(*this); 
}

void VariableExpression::accept(ExpressionVisitor& visitor) const
{
    visitor.visit(*this); 
}

void BinaryExpression::accept(ExpressionVisitor& visitor) const
{
    visitor.visit(*this); 
}

void UnaryExpression::accept(ExpressionVisitor& visitor) const
{
    visitor.visit(*this);
}

void FunctionCallExpression::accept(ExpressionVisitor& visitor) const
{
    visitor.visit(*this);
}

void AccessMemberExpression::accept(ExpressionVisitor& visitor) const
{
    visitor.visit(*this);
}
