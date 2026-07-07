#include"visitors.hpp"
#include"expression.hpp"
#include"scope.hpp"
#include"variable.hpp"
#include"parser.hpp"

void NodeMaker::visit(const AssignmentNode& node)
{
    ExpressionEvaluator evaluator {scope};
    double new_value = evaluator.evaluate(*node.value_expr);
    
    auto existing_var = scope.lookup(node.name);
    auto numeric_var = std::dynamic_pointer_cast<NumericDeclaration>(existing_var);
    if (!numeric_var) 
    {
        throw std::runtime_error("Runtime Error: " + node.name + " is not a mutable numeric variable.");
    }

    numeric_var->setValue(new_value);

    // DUBUG
    //
    std::cout << "[ASSIGNMENT] " << existing_var->getName() << " <-- ";
    PrintVisitor print_visitor {};
    existing_var->accept(print_visitor);
    std::cout << std::endl;
}

void NodeMaker::visit(const DeclarationNode& node)
{
    double resolved_value = 0.0;

    if (node.initializer) 
    {
        ExpressionEvaluator evaluator {scope};
        resolved_value = evaluator.evaluate(*node.initializer);
    }

    std::shared_ptr<Declaration> concrete_decl = nullptr;

    if (node.type == "i") 
    {
        concrete_decl = std::make_shared<Numeric<int>>(node.name, static_cast<int>(resolved_value));
    } else if (node.type == "d") 
    {
        concrete_decl = std::make_shared<Numeric<double>>(node.name, resolved_value);
    } else if (node.type == "f") 
    {
        concrete_decl = std::make_shared<Numeric<float>>(node.name, static_cast<float>(resolved_value));
    }

    if (!concrete_decl)
    {
        throw std::runtime_error("Error: Could not declare variable of type: " + node.type);
    }

    scope.define(node.name, concrete_decl);

    // DUBUG
    //
    std::cout << "[DECLARATION] " << concrete_decl->getName() << " <-- ";
    PrintVisitor print_visitor {};
    concrete_decl->accept(print_visitor);
    std::cout << std::endl;
}

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