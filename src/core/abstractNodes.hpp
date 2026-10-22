#ifndef ANODES_HPP
#define ANODES_HPP

#include<string>
#include<vector>
#include<memory>

class NodeVisitor;
class Expression;
class NamedExpression;
class MultiExpression;
class DeclarationNode;
class FunctionCallExpression;

// TODO: Pass to node constructors the line where it was written for better error messages  

// Base class for all abstract nodes
class ASTNode
{
public:
    virtual void accept(NodeVisitor& visitor) const = 0;
    virtual ~ASTNode() = default;
};

class DeclarableNode : public ASTNode
{
public:
    std::string type;
    std::string name;

    explicit DeclarableNode(std::string tp, std::string _name);
};

class AssignmentNode : public ASTNode 
{
public:
    std::string line;                        
    std::unique_ptr<NamedExpression> assignee;                        
    std::unique_ptr<Expression> value_expr; 

    void accept(NodeVisitor& visitor) const override;
    explicit AssignmentNode(std::unique_ptr<NamedExpression> _assignee, std::unique_ptr<Expression> val);
    ~AssignmentNode();
};

class DeclarationNode : public DeclarableNode
{
public:
    std::string line;                        
    std::unique_ptr<Expression> initializer;

    void accept(NodeVisitor& visitor) const override;
    explicit DeclarationNode(std::string t, std::string n, std::unique_ptr<Expression> init);
    ~DeclarationNode();
};

class ObjectDeclarationNode : public DeclarableNode
{
public:
    std::string line;                        
    std::unique_ptr<MultiExpression> initializers;

    void accept(NodeVisitor& visitor) const override;
    explicit ObjectDeclarationNode(std::string tn, std::string n, std::unique_ptr<MultiExpression> init);
    ~ObjectDeclarationNode();
};

class FunctionCallNode : public ASTNode
{
public:
    std::string line;                        
    std::string name;
    std::unique_ptr<FunctionCallExpression> expr;
    void accept(NodeVisitor& visitor) const override;

    explicit FunctionCallNode(std::string n, std::unique_ptr<FunctionCallExpression> _expr);
    ~FunctionCallNode();
};

class FunctionDeclarationNode : public ASTNode
{
public:
    std::string line;                        
    std::string return_type;
    std::string name;
    std::vector<std::unique_ptr<DeclarableNode>> arg_nodes;
    std::vector<std::unique_ptr<ASTNode>> body_nodes;

    void accept(NodeVisitor& visitor) const override;

    explicit FunctionDeclarationNode(
        std::string rt, 
        std::string n, 
        std::vector<std::unique_ptr<DeclarableNode>> _args,
        std::vector<std::unique_ptr<ASTNode>> bdnds);

    ~FunctionDeclarationNode();

};

class ElseNode : public ASTNode
{
public:
    std::string line;                        
    std::vector<std::unique_ptr<ASTNode>> body_nodes;

    void accept(NodeVisitor& visitor) const override;

    explicit ElseNode(std::vector<std::unique_ptr<ASTNode>> bdnds);
    ~ElseNode();
};

class IfNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    std::unique_ptr<Expression> condition_expr;
    std::unique_ptr<ElseNode> else_nd;

    void accept(NodeVisitor& visitor) const override;

    explicit IfNode(
        std::vector<std::unique_ptr<ASTNode>> bdnds,
        std::unique_ptr<Expression> c_expr,
        std::unique_ptr<ElseNode> _else_nd );
    
    ~IfNode();
};

class WhileNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    std::unique_ptr<Expression> condition_expr;

    void accept(NodeVisitor& visitor) const override;

    explicit WhileNode(
        std::vector<std::unique_ptr<ASTNode>> bdnds,
        std::unique_ptr<Expression> c_expr);

    ~WhileNode();
};

class ReturnNode : public ASTNode
{
public:
    std::unique_ptr<Expression> ret_expr;

    void accept(NodeVisitor& visitor) const override;

    explicit ReturnNode( std::unique_ptr<Expression> r_expr );

    ~ReturnNode();
};

class ObjectSignatureNode : public ASTNode
{
public:
    std::string line;                        
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> member_variables;

    void accept(NodeVisitor& visitor) const override;

    explicit ObjectSignatureNode( 
        std::string n,
        std::vector<std::unique_ptr<ASTNode>> mv );

    ~ObjectSignatureNode();
};

// Holds c++ code to be executed
class EmbeddedFunctionNode : public ASTNode
{
public:
    virtual void accept(NodeVisitor& visitor) const override = 0;
};

class EmbeddedPrintFunctionNode : public EmbeddedFunctionNode
{
public:
    void accept(NodeVisitor& visitor) const override;
};

class EmbeddedCastFunctionNode : public EmbeddedFunctionNode
{
public:
    std::string tp;
    void accept(NodeVisitor& visitor) const override;

    explicit EmbeddedCastFunctionNode(std::string _tp) : tp( _tp ) {}
};

#endif