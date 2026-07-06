#include<iostream>

#include"builder.hpp"
#include"expression.hpp"
#include"parser.hpp"
#include"variable.hpp"
#include"scope.hpp"
#include"visitors.hpp"

void Builder::buildProgram(const std::vector<std::unique_ptr<ASTNode>>& ast) 
{
    for (const auto& node_ptr : ast) 
    {
        NodeMaker node_visitor {scope};

        try
        {        
            node_ptr->accept(node_visitor);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
}