#include"abstractNodes.hpp"
#include"expression.hpp"
#include"visitorsBase.hpp"

AssignmentNode::AssignmentNode(std::unique_ptr<NamedExpression> _assignee, std::unique_ptr<Expression> val)
    : assignee(std::move(_assignee)), value_expr(std::move(val)) {}

DeclarationNode::DeclarationNode(ImplementedType t, std::string n, std::unique_ptr<Expression> init)
    : type(t), name(std::move(n)), initializer(std::move(init)) {}

ObjectDeclarationNode::ObjectDeclarationNode(std::string tn, std::string n, std::vector<std::unique_ptr<Expression>> init)
    : type_name(tn), name(std::move(n)), initializers(std::move(init)) {}

FunctionCallNode::FunctionCallNode(
    std::string n,
    std::unique_ptr<FunctionCallExpression> _expr) :
        name(std::move(n)),
        expr(std::move(_expr)) {}

FunctionDeclarationNode::FunctionDeclarationNode(
    ImplementedType rt, 
    std::string n, 
    std::vector<std::unique_ptr<DeclarationNode>> _args,
    std::vector<std::unique_ptr<ASTNode>> bdnds) :
        return_type(rt), 
        name(std::move(n)), 
        arg_nodes(std::move(_args)),
        body_nodes(std::move(bdnds) ) {}

ElseNode::ElseNode(
    std::vector<std::unique_ptr<ASTNode>> bdnds) :
        body_nodes(std::move( bdnds )) {}

IfNode::IfNode(
    std::vector<std::unique_ptr<ASTNode>> bdnds,
    std::unique_ptr<Expression> c_expr,
    std::unique_ptr<ElseNode> _else_nd ) :
        body_nodes(std::move( bdnds ) ),
        condition_expr(std::move( c_expr ) ),
        else_nd(std::move(_else_nd)) {}

WhileNode::WhileNode(
    std::vector<std::unique_ptr<ASTNode>> bdnds,
    std::unique_ptr<Expression> c_expr) :
        body_nodes(std::move( bdnds ) ),
        condition_expr(std::move( c_expr ) ) {}

ReturnNode::ReturnNode( std::unique_ptr<Expression> r_expr ) : ret_expr(std::move(r_expr)) {}

ObjectSignatureNode::ObjectSignatureNode( 
    std::string n,
    std::vector<std::unique_ptr<ASTNode>> mv ) : 
    name(std::move(n)),
    member_variables(std::move(mv)) {}

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

void ObjectSignatureNode::accept(NodeVisitor& visitor) const 
{
    visitor.visit(*this);
}

void ObjectDeclarationNode::accept(NodeVisitor& visitor) const 
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
