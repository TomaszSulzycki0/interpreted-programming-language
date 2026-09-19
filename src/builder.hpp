#ifndef BUILDER_HPP
#define BUILDER_HPP

#include<memory>
#include<vector>
#include"visitorsBase.hpp"

class Scope;
class Expression;
class ASTNode;

class Builder 
{
private:
    std::shared_ptr<Scope> scope;
public:
    explicit Builder(std::shared_ptr<Scope> s) : scope(std::move(s)) {}
    void buildProgram(const std::vector<std::unique_ptr<ASTNode>>& ast); 
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


#endif