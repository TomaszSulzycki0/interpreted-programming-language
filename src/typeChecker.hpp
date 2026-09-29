#ifndef TYPE_CHECKER_HPP
#define TYPE_CHECKER_HPP

#include<vector>
#include<memory>

#include"visitorsBase.hpp"
#include"implementedType.hpp"

class ASTNode;
class SemanticScope;

// Type safety checker module
// Checks for matching types in declarations, assignments, function call return values, function arguments 
// variable / function redeclarations, undefined references etc.
class TypeChecker
{
private: 
    std::shared_ptr<SemanticScope> scope;

    // If checker throws at any point, 
    // it continues checking nodes but marks itself as non executable
    bool is_well_typed = true;

public:

    // Checks type safety of the AST and sets the flag returned by isASTWellTyped() accordingly
    void run_check(const std::vector<std::unique_ptr<ASTNode>>& nodes);

    // Returns the result of run_check() 
    bool isASTWellTyped() const { return is_well_typed; }

    explicit TypeChecker(std::shared_ptr<SemanticScope> s) : scope(std::move(s)) {}
};

// Visitor that checks type safety of individual AST nodes
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

// Visitor that evaluates the type of expressions
class ExpressionTypeEvaluator final : public ExpressionVisitor 
{
private:
    std::shared_ptr<SemanticScope> scope;
    ImplementedType last_evaluated_type;
public:

    // Evaluate the type of an expression in the current scope
    ImplementedType evaluateType(const Expression& expr);
    
    void visit(const LiteralExpression& expr) override;
    void visit(const VariableExpression& expr) override;
    void visit(const BinaryExpression& expr) override;
    void visit(const UnaryExpression& expr) override;
    void visit(const FunctionCallExpression& expr) override;

    explicit ExpressionTypeEvaluator(std::shared_ptr<SemanticScope> s) : scope(std::move(s)) {}
};


#endif
