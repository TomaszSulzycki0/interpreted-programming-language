#include"AbstractNodes.hpp"
#include"visitors.hpp"

void AssignmentNode::accept(NodeVisitor& visitor) const 
{
    visitor.visit(*this); 
}

void DeclarationNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this); 
}

void FunctionDeclarationNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this); 
}

void FunctionCallNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this); 
}
