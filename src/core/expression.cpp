#include"core/expression.hpp"
#include"core/declarationBase.hpp"
#include"core/scope.hpp"
#include"core/visitorsBase.hpp"
#include"parser/parser.hpp"

BinaryExpression::BinaryExpression(std::string _operator, std::unique_ptr<Expression> _left, std::unique_ptr<Expression> _right)
    : expr_operator(std::move(_operator)), expr_left(std::move(_left)), expr_right(std::move(_right)) {}

BinaryExpression::BinaryExpression( const BinaryExpression& other) 
        :   expr_operator(other.expr_operator), 
            expr_left(other.expr_left ? other.expr_left->clone() : nullptr),
            expr_right(other.expr_right ? other.expr_right->clone() : nullptr) {}

LiteralExpression::LiteralExpression(std::string v, std::string _tp) : value(std::move(v)), tp(std::move(_tp)) {}

LiteralExpression::LiteralExpression(const LiteralExpression& other) : value(other.value), tp(other.tp) {}

VariableExpression::VariableExpression(std::string n) : NamedExpression(n) {}

VariableExpression::VariableExpression(const VariableExpression& other) : NamedExpression(other.name) {}

UnaryExpression::UnaryExpression(std::string op, std::unique_ptr<Expression> _child) 
    : unary_op(std::move(op)), child(std::move(_child)) {}

UnaryExpression::UnaryExpression(const UnaryExpression& other) : unary_op(other.unary_op), child( other.child ? other.child->clone() : nullptr ) {}

FunctionCallExpression::FunctionCallExpression(std::string n, std::vector<std::unique_ptr<Expression>> _args) 
    : NamedExpression(n), args( std::move(_args)) {}

FunctionCallExpression::FunctionCallExpression(const FunctionCallExpression& other) : NamedExpression(other.name)
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

AccessMemberExpression::AccessMemberExpression( std::unique_ptr<NamedExpression> expr,
        std::string mem ) :
        NamedExpression(mem), left_side_expr(std::move(expr))  {}

AccessMemberExpression::AccessMemberExpression(const AccessMemberExpression& other) : NamedExpression(other.name)
{
    if ( other.left_side_expr )
    {
        auto cloned = other.left_side_expr->clone();
        
        left_side_expr = std::unique_ptr<NamedExpression>(
            dynamic_cast<NamedExpression*>(cloned.release())
        );
    }
    else
    {
        left_side_expr = nullptr;
    }
    
}

MultiExpression::MultiExpression(std::vector<std::unique_ptr<Expression>> expressions) : expr_list(std::move(expressions)) {}

MultiExpression::MultiExpression(const MultiExpression& other) 
{
    expr_list.reserve( other.expr_list.size() );

    for (const auto& expr : other.expr_list) 
    {
        if (expr) 
        {
            expr_list.push_back(expr->clone());
        } 
        else 
        {
            expr_list.push_back(nullptr);
        }
    }
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

void MultiExpression::accept(ExpressionVisitor& visitor) const
{
    visitor.visit(*this);
}