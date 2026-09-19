#include"expression.hpp"
#include"declarationBase.hpp"
#include"scope.hpp"
#include"visitorsBase.hpp"

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


