#include"visitors.hpp"
#include"expression.hpp"
#include"scope.hpp"
#include"variable.hpp"

void PrintVisitor::visit(const NumericDeclaration& num_decl) 
{
    if (num_decl.isFloatingPoint()) 
    {
        std::cout << "(floating point) " << num_decl.asDouble();
    } else 
    {
        std::cout << "(integer) " << num_decl.asInteger();
    }
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