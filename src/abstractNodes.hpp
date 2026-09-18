#ifndef ANODES_HPP
#define ANODES_HPP

#include<string>
#include<vector>
#include<memory>

#include"expression.hpp"

class NodeVisitor;
class Expression;
class DeclarationNode;
enum class ImplementedType;

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
    std::string name;                        
    std::unique_ptr<Expression> value_expr; 

    void accept(NodeVisitor& visitor) const override;
    explicit AssignmentNode(std::string n, std::unique_ptr<Expression> val)
        : name(std::move(n)), value_expr(std::move(val)) {}
};

class DeclarationNode : public ASTNode
{
public:
    std::string line;                        
    ImplementedType type;
    std::string name;
    std::unique_ptr<Expression> initializer;

    void accept(NodeVisitor& visitor) const override;
    explicit DeclarationNode(ImplementedType t, std::string n, std::unique_ptr<Expression> init)
        : type(t), name(std::move(n)), initializer(std::move(init)) {}
};

class FunctionCallNode : public ASTNode
{
public:
    std::string line;                        
    std::string name;
    std::unique_ptr<FunctionCallExpression> expr;
    void accept(NodeVisitor& visitor) const override;

    explicit FunctionCallNode(
        std::string n,
        std::unique_ptr<FunctionCallExpression> _expr) :
            name(std::move(n)),
            expr(std::move(_expr)) {}
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
        std::vector<std::unique_ptr<ASTNode>> bdnds) :
            return_type(rt), 
            name(std::move(n)), 
            arg_nodes(std::move(_args)),
            body_nodes(std::move(bdnds) ) {}

};

class ElseNode : public ASTNode
{
public:
    std::string line;                        
    std::vector<std::unique_ptr<ASTNode>> body_nodes;

    void accept(NodeVisitor& visitor) const override;

    explicit ElseNode(
        std::vector<std::unique_ptr<ASTNode>> bdnds) :
            body_nodes(std::move( bdnds )) {}
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
        std::unique_ptr<ElseNode> _else_nd ) :
            body_nodes(std::move( bdnds ) ),
            condition_expr(std::move( c_expr ) ),
            else_nd(std::move(_else_nd)) {}
};

class WhileNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    std::unique_ptr<Expression> condition_expr;

    void accept(NodeVisitor& visitor) const override;

    explicit WhileNode(
        std::vector<std::unique_ptr<ASTNode>> bdnds,
        std::unique_ptr<Expression> c_expr) :
            body_nodes(std::move( bdnds ) ),
            condition_expr(std::move( c_expr ) ) {}
};

class ReturnNode : public ASTNode
{
public:
    std::unique_ptr<Expression> ret_expr;

    void accept(NodeVisitor& visitor) const override;

    explicit ReturnNode( std::unique_ptr<Expression> r_expr ) : ret_expr(std::move(r_expr)) {}
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