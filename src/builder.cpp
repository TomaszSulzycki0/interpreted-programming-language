#include"builder.hpp"
#include"expression.hpp"
#include"parser.hpp"
#include"variable.hpp"

std::shared_ptr<Declaration> Builder::build(const DeclarationNode& node) 
{
    double resolved_value = 0.0;

    if (node.initializer) 
    {
        ExpressionEvaluator evaluator(scope);
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

    scope.define(node.name, concrete_decl);
    return concrete_decl;
}