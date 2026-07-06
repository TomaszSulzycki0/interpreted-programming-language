#include<iostream>

#include"builder.hpp"
#include"expression.hpp"
#include"parser.hpp"
#include"variable.hpp"
#include"scope.hpp"
#include"visitors.hpp"

void Builder::buildProgram(const std::vector<std::unique_ptr<DeclarationNode>>& ast) 
{
    for (const auto& node_ptr : ast) 
    {
        auto real_declaration = buildNode(*node_ptr);
        
        if (real_declaration == nullptr) 
        {
            throw std::runtime_error("Error: Could not create variable of type: " + node_ptr->type); 
        }

        // DUBUG
        //
        std::cout << real_declaration->getName() << " : ";
        PrintVisitor visitor;
        real_declaration->accept(visitor);
        std::cout << std::endl;
        
    }
}

std::shared_ptr<Declaration> Builder::buildNode(const DeclarationNode& node) 
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