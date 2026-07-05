#include"expression.hpp"
#include"declarationBase.hpp"

void LiteralExpression::accept(ExpressionVisitor& visitor) const 
{
    visitor.visit(*this); 
}

void VariableExpression::accept(ExpressionVisitor& visitor) const
{
    visitor.visit(*this); 
}

double ExpressionEvaluator::evaluate(const Expression& expr) 
{
    expr.accept(*this);
    return last_evaluated_value;
}

void ExpressionEvaluator::visit(const LiteralExpression& expr)  
{
    last_evaluated_value = std::stod(expr.value);
}

void ExpressionEvaluator::visit(const VariableExpression& expr) 
{
    auto decl = scope.lookup(expr.name);
    auto num_decl = std::dynamic_pointer_cast<NumericDeclaration>(decl);
    if (!num_decl) 
    {
        throw std::runtime_error("Error: " + expr.name + " is not numeric.");
    }
    
    last_evaluated_value = num_decl->asDouble();
}