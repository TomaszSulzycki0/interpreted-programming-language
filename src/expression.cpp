#include"expression.hpp"
#include"declarationBase.hpp"
#include"scope.hpp"
#include"visitors.hpp"

void LiteralExpression::accept(ExpressionVisitor& visitor) const 
{
    visitor.visit(*this); 
}

void VariableExpression::accept(ExpressionVisitor& visitor) const
{
    visitor.visit(*this); 
}