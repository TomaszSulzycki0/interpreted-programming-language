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
class WhileNode;

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
    virtual void visit(const WhileNode& node) = 0;

    virtual ~NodeVisitor() = default;
};

class NodeMaker : public NodeVisitor
{
private:
    std::shared_ptr<Scope> scope;
public:
    void visit(const AssignmentNode& node) override;
    void visit(const DeclarationNode& node) override;
    void visit(const FunctionDeclarationNode& node) override;
    void visit(const FunctionCallNode& node) override;
    void visit(const IfNode& node) override;
    void visit(const WhileNode& node) override;

    explicit NodeMaker(std::shared_ptr<Scope> s) : scope(std::move(s)) {}
};

class NodeTypeChecker : public NodeVisitor
{
private:
    std::shared_ptr<SemanticScope> scope;

    bool isNumeric(const std::string& tp) const
    {
        if ( tp == "int" || tp == "float" || tp == "double" || tp == "bool" )
        {
            return true;
        }
        return false;
    }

    int getTypeConversionRank(const std::string& tp) const
    {
        if ( tp == "bool" )     return 0;
        if ( tp == "int" )      return 1;
        if ( tp == "float" )    return 2;
        if ( tp == "double" )   return 3;

        return -1;
    }

public:
    void visit(const AssignmentNode& node) override;
    void visit(const DeclarationNode& node) override;
    void visit(const FunctionDeclarationNode& node) override;
    void visit(const FunctionCallNode& node) override;
    void visit(const IfNode& node) override;
    void visit(const WhileNode& node) override;

    explicit NodeTypeChecker(std::shared_ptr<SemanticScope> s) : scope(std::move(s)) {}
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

class ExpressionEvaluator final : public ExpressionVisitor 
{
private:
    std::shared_ptr<Scope> scope;
    RuntimeValue last_evaluated_value = 0.0;
    RuntimeValue resolveOperator(   const RuntimeValue& v_left, 
                                    const RuntimeValue& v_right,
                                    const std::string& op) const;

public:
    RuntimeValue evaluate(const Expression& expr);
    void visit(const LiteralExpression& expr) override;
    void visit(const VariableExpression& expr) override;
    void visit(const BinaryExpression& expr) override;
    void visit(const UnaryExpression& expr) override;
    void visit(const FunctionCallExpression& expr) override;

    explicit ExpressionEvaluator(std::shared_ptr<Scope> s) : scope(std::move(s)) {}
};

class ExpressionTypeEvaluator final : public ExpressionVisitor 
{
private:
    std::shared_ptr<SemanticScope> scope;
    std::string last_evaluated_type;
public:
    std::string evaluateType(const Expression& expr);
    void visit(const LiteralExpression& expr) override;
    void visit(const VariableExpression& expr) override;
    void visit(const BinaryExpression& expr) override;
    void visit(const UnaryExpression& expr) override;
    void visit(const FunctionCallExpression& expr) override;

    explicit ExpressionTypeEvaluator(std::shared_ptr<SemanticScope> s) : scope(std::move(s)) {}
};

#endif