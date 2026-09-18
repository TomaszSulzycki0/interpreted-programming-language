#ifndef VISITORS_HPP
#define VISITORS_HPP

#include<iostream>
#include<memory>
#include<map>
#include<variant>
#include<functional>

#include"implementedType.hpp"

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

class NodeMaker : public NodeVisitor
{
private:
    std::shared_ptr<Scope> scope;

    std::unique_ptr<Expression> return_expression = nullptr;
    std::shared_ptr<Scope> return_expression_scope = nullptr;

public:
    void visit(const AssignmentNode& node) override;
    void visit(const DeclarationNode& node) override;
    void visit(const FunctionDeclarationNode& node) override;
    void visit(const FunctionCallNode& node) override;
    void visit(const IfNode& node) override;
    void visit(const ElseNode& node) override;
    void visit(const WhileNode& node) override;
    void visit(const ReturnNode& node) override;
    void visit(const EmbeddedPrintFunctionNode& node) override;
    void visit(const EmbeddedCastFunctionNode& node) override;

    bool reached_return_statement = false;

    Expression* getReturnExpression() const { return return_expression.get(); }

    std::shared_ptr<Scope> getReturnExpressionScope() const { return return_expression_scope; }

    explicit NodeMaker(std::shared_ptr<Scope> s) : scope(std::move(s)) {}
};

class NodeTypeChecker : public NodeVisitor
{
private:
    std::shared_ptr<SemanticScope> scope;

    bool isReturnSafe = false;

public:
    void visit(const AssignmentNode& node) override;
    void visit(const DeclarationNode& node) override;
    void visit(const FunctionDeclarationNode& node) override;
    void visit(const FunctionCallNode& node) override;
    void visit(const IfNode& node) override;
    void visit(const ElseNode& node) override;
    void visit(const WhileNode& node) override;
    void visit(const ReturnNode& node) override;
    void visit(const EmbeddedPrintFunctionNode&) override {};
    void visit(const EmbeddedCastFunctionNode&) override {};

    ImplementedType fn_return_type = ImplementedType::NULL_TYPE;
    
    static void checkTypeUpCasting(ImplementedType from, ImplementedType to);
    static int getTypeConversionRank(ImplementedType tp);
    static ImplementedType getInverseTypeConversionRank(int rank);
    static bool isNumeric(ImplementedType tp);

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
    ImplementedType last_evaluated_type;
public:
    ImplementedType evaluateType(const Expression& expr);
    void visit(const LiteralExpression& expr) override;
    void visit(const VariableExpression& expr) override;
    void visit(const BinaryExpression& expr) override;
    void visit(const UnaryExpression& expr) override;
    void visit(const FunctionCallExpression& expr) override;

    explicit ExpressionTypeEvaluator(std::shared_ptr<SemanticScope> s) : scope(std::move(s)) {}
};

#endif