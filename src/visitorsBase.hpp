#ifndef VISITORS_HPP
#define VISITORS_HPP

#include<iostream>
#include<memory>
#include<map>
#include<variant>
#include<functional>

using RuntimeValue = std::variant<int, float, double, bool, std::string>;

class Declaration;
class NumericDeclaration;
class StringDeclaration;
class FunctionDeclaration;

class Expression;
class BinaryExpression;
class LiteralExpression;
class VariableExpression;
class UnaryExpression;
class FunctionCallExpression;

class ASTNode;
class AssignmentNode;
class DeclarationNode;
class FunctionDeclarationNode;
class FunctionCallNode;
class IfNode;
class ElseNode;
class WhileNode;
class ReturnNode;
class EmbeddedPrintFunctionNode;
class EmbeddedCastFunctionNode;

class Scope;
class SemanticScope;

class DeclarationVisitor 
{
public:
    virtual void visit(const NumericDeclaration& num_decl) = 0;
    virtual void visit(const StringDeclaration& str_decl) = 0;
    virtual void visit(const FunctionDeclaration& fn_decl) = 0;

    virtual ~DeclarationVisitor() = default;
};

class PrintVisitor final : public DeclarationVisitor 
{
public:
    void visit(const NumericDeclaration& num_decl) override;
    void visit(const StringDeclaration& str_decl) override;
    void visit(const FunctionDeclaration& fn_decl) override;
};

class NodeVisitor
{
public:
    virtual void visit(const AssignmentNode& node) = 0;
    virtual void visit(const DeclarationNode& node) = 0;    
    virtual void visit(const FunctionDeclarationNode& node) = 0;
    virtual void visit(const FunctionCallNode& node) = 0;
    virtual void visit(const IfNode& node) = 0;
    virtual void visit(const ElseNode& node) = 0;
    virtual void visit(const WhileNode& node) = 0;
    virtual void visit(const ReturnNode& node) = 0;
    virtual void visit(const EmbeddedPrintFunctionNode& node) = 0;
    virtual void visit(const EmbeddedCastFunctionNode& node) = 0;

    virtual ~NodeVisitor() = default;
};

class ExpressionVisitor 
{
public:
    virtual void visit(const LiteralExpression& expr) = 0;
    virtual void visit(const VariableExpression& expr) = 0;
    virtual void visit(const BinaryExpression& expr) = 0;
    virtual void visit(const UnaryExpression& expr) = 0;
    virtual void visit(const FunctionCallExpression& expr) = 0;

    virtual ~ExpressionVisitor() = default;
};

#endif