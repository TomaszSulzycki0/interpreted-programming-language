#include<iostream>

#include"builder.hpp"
#include"parser.hpp"
#include"visitors.hpp"
#include"AbstractNodes.hpp"

void Builder::buildProgram(const std::vector<std::unique_ptr<ASTNode>>& ast) 
{
    NodeMaker node_visitor {scope};

    for (const auto& node_ptr : ast) 
    {        
        node_ptr->accept(node_visitor);
    }
}