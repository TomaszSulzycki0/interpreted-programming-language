#ifndef ANODES_HPP
#define ANODES_HPP

#include<string>
#include<vector>
#include<memory>

class NodeVisitor;
class Expression;
class NamedExpression;
class DeclarationNode;
class FunctionCallExpression;
enum class ImplementedType;

// TODO: Pass to node constructors the line where it was written for better error messages  

// Base class for all abstract nodes
class ASTNode
{
public:
    virtual void accept(NodeVisitor& visitor) const = 0;
    virtual ~ASTNode() = default;
};

class AssignmentNode : public ASTNode 
{
public:
    std::string line;                        
    std::unique_ptr<NamedExpression> assignee;                        
    std::unique_ptr<Expression> value_expr; 

    void accept(NodeVisitor& visitor) const override;
    explicit AssignmentNode(std::unique_ptr<NamedExpression> _assignee, std::unique_ptr<Expression> val);
};

class DeclarationNode : public ASTNode
{
public:
    std::string line;                        
    ImplementedType type;
    std::string name;
    std::unique_ptr<Expression> initializer;

    void accept(NodeVisitor& visitor) const override;
    explicit DeclarationNode(ImplementedType t, std::string n, std::unique_ptr<Expression> init);
};

class ObjectDeclarationNode : public ASTNode
{
public:
    std::string line;                        
    std::string type_name;
    std::string name;
    std::vector<std::unique_ptr<Expression>> initializers;

    void accept(NodeVisitor& visitor) const override;
    explicit ObjectDeclarationNode(std::string tn, std::string n, std::vector<std::unique_ptr<Expression>> init);
};

class FunctionCallNode : public ASTNode
{
public:
    std::string line;                        
    std::string name;
    std::unique_ptr<FunctionCallExpression> expr;
    void accept(NodeVisitor& visitor) const override;

    explicit FunctionCallNode(std::string n, std::unique_ptr<FunctionCallExpression> _expr);
};

class FunctionDeclarationNode : public ASTNode
{
public:
    std::string line;                        
    ImplementedType return_type;
    std::string name;
    std::vector<std::unique_ptr<DeclarationNode>> arg_nodes;
    std::vector<std::unique_ptr<ASTNode>> body_nodes;

    void accept(NodeVisitor& visitor) const override;

    explicit FunctionDeclarationNode(
        ImplementedType rt, 
        std::string n, 
        std::vector<std::unique_ptr<DeclarationNode>> _args,
        std::vector<std::unique_ptr<ASTNode>> bdnds);

};

class ElseNode : public ASTNode
{
public:
    std::string line;                        
    std::vector<std::unique_ptr<ASTNode>> body_nodes;

    void accept(NodeVisitor& visitor) const override;

    explicit ElseNode(std::vector<std::unique_ptr<ASTNode>> bdnds);
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
};

class ReturnNode : public ASTNode
{
public:
    std::unique_ptr<Expression> ret_expr;

    void accept(NodeVisitor& visitor) const override;

    explicit ReturnNode( std::unique_ptr<Expression> r_expr );
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
    ImplementedType tp;
    void accept(NodeVisitor& visitor) const override;

    explicit EmbeddedCastFunctionNode(ImplementedType _tp) : tp( _tp ) {}
};

#endif