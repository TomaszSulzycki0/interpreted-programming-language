#include"abstractNodes.hpp"
#include"visitorsBase.hpp"

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

void IfNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this);
}

void ElseNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this);
}

void WhileNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this);
}

void ReturnNode::accept(NodeVisitor& visitor) const 
{
    visitor.visit(*this);
}

void EmbeddedPrintFunctionNode::accept(NodeVisitor& visitor) const 
{
    visitor.visit(*this);
}

void EmbeddedCastFunctionNode::accept(NodeVisitor& visitor) const 
{
    visitor.visit(*this);
}
