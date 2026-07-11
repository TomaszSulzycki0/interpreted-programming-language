#ifndef VISITORS_HPP
#define VISITORS_HPP

#include<iostream>
#include<memory>
#include<variant>

using RuntimeValue = std::variant<int, float, double, std::string>;

class Declaration;
class NumericDeclaration;
class StringDeclaration;

class Expression;
class BinaryExpression;
class LiteralExpression;
class VariableExpression;

class ASTNode;
class AssignmentNode;
class DeclarationNode;

class Scope;

class DeclarationVisitor 
{
public:
    virtual void visit(const NumericDeclaration& num_decl) = 0;
    virtual void visit(const StringDeclaration& str_decl) = 0;

    virtual ~DeclarationVisitor() = default;
};

class PrintVisitor final : public DeclarationVisitor 
{
public:
    void visit(const NumericDeclaration& num_decl) override;
    void visit(const StringDeclaration& str_decl) override;
};

class NodeVisitor
{
public:
    virtual void visit(const AssignmentNode& node) = 0;
    virtual void visit(const DeclarationNode& node) = 0;

    virtual ~NodeVisitor() = default;
};

class NodeMaker : public NodeVisitor
{
private:
    Scope& scope;
public:
    void visit(const AssignmentNode& node) override;
    void visit(const DeclarationNode& node) override;

    explicit NodeMaker(Scope& s) : scope(s) {}
};

class ExpressionVisitor 
{
public:
    virtual void visit(const LiteralExpression& expr) = 0;
    virtual void visit(const VariableExpression& expr) = 0;
    virtual void visit(const BinaryExpression& expr) = 0;

    virtual ~ExpressionVisitor() = default;
};

class ExpressionEvaluator final : public ExpressionVisitor 
{
private:
    const Scope& scope;
    RuntimeValue last_evaluated_value = 0.0;
    RuntimeValue resolveOperator(   const RuntimeValue& v_left, 
                                    const RuntimeValue& v_right,
                                    const std::string& op) const;

public:
    RuntimeValue evaluate(const Expression& expr);
    void visit(const LiteralExpression& expr) override;
    void visit(const VariableExpression& expr) override;
    void visit(const BinaryExpression& expr) override;

    explicit ExpressionEvaluator(const Scope& s) : scope(s) {}
};

#endif