#ifndef ANODES_HPP
#define ANODES_HPP

#include<string>
#include<vector>
#include<memory>

#include"expression.hpp"

class NodeVisitor;
class Expression;
class DeclarationNode;

class ASTNode
{
public:
    virtual void accept(NodeVisitor& visitor) const = 0;
    virtual ~ASTNode() = default;
};

class AssignmentNode : public ASTNode 
{
public:
    std::string name;                        
    std::unique_ptr<Expression> value_expr; 

    void accept(NodeVisitor& visitor) const override;
    explicit AssignmentNode(std::string n, std::unique_ptr<Expression> val)
        : name(std::move(n)), value_expr(std::move(val)) {}
};

class DeclarationNode : public ASTNode
{
public:
    std::string type;
    std::string name;
    std::unique_ptr<Expression> initializer;

    void accept(NodeVisitor& visitor) const override;
    explicit DeclarationNode(std::string t, std::string n, std::unique_ptr<Expression> init)
        : type(std::move(t)), name(std::move(n)), initializer(std::move(init)) {}
};

class FunctionCallNode : public ASTNode
{
public:
    std::string name;
    std::vector<std::string> args;
    void accept(NodeVisitor& visitor) const override;
};

class FunctionDeclarationNode : public ASTNode
{
public:
    std::string return_type;
    std::string name;
    std::vector<std::unique_ptr<DeclarationNode>> arg_nodes;
    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    std::unique_ptr<Expression> return_expr;

    void accept(NodeVisitor& visitor) const override;

    explicit FunctionDeclarationNode(
        std::string rt, 
        std::string n, 
        std::vector<std::unique_ptr<DeclarationNode>> _args,
        std::vector<std::unique_ptr<ASTNode>> bdnds,
        std::unique_ptr<Expression> ret_expr) :
            return_type(std::move(rt)), 
            name(std::move(n)), 
            arg_nodes(std::move(_args)),
            body_nodes(std::move(bdnds)),
            return_expr(std::move(ret_expr)) {}

};

class IfNode : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    std::unique_ptr<Expression> condition_expr;

    void accept(NodeVisitor& visitor) const override;

    explicit IfNode(
        std::vector<std::unique_ptr<ASTNode>> bdnds,
        std::unique_ptr<Expression> c_expr) :
            body_nodes(std::move( bdnds ) ),
            condition_expr(std::move( c_expr ) ) {}
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


#endif