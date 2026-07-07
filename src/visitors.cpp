#include"visitors.hpp"
#include"expression.hpp"
#include"scope.hpp"
#include"numericVariable.hpp"
#include"parser.hpp"
#include"stringVariable.hpp"

void NodeMaker::visit(const AssignmentNode& node)
{
    ExpressionEvaluator evaluator {scope};
    RuntimeValue new_value = evaluator.evaluate(*node.value_expr);
    
    auto existing_var = scope.lookup(node.name);
    auto target_var = std::dynamic_pointer_cast<ValueDeclaration>(existing_var);
    if (!target_var) 
    {
        throw std::runtime_error("Runtime Error: " + node.name + " is not a mutable variable.");
    }

    std::visit([&](auto&&) {
        target_var->setValue(new_value);
    }, new_value);

    // DUBUG
    //
    std::cout << "[ASSIGNMENT] " << existing_var->getName() << " <-- ";
    PrintVisitor print_visitor {};
    existing_var->accept(print_visitor);
    std::cout << std::endl;
}

void NodeMaker::visit(const DeclarationNode& node)
{
    RuntimeValue raw_value; 
    if (node.initializer) 
    {
        ExpressionEvaluator evaluator {scope};
        raw_value = evaluator.evaluate(*node.initializer);
    }
    else 
    {
        if (node.type == "i") raw_value = 0;
        else if (node.type == "d" || node.type == "f") raw_value = 0.0;
        else if (node.type == "s") raw_value = "";
    }

    std::shared_ptr<Declaration> concrete_decl = nullptr;

    std::visit([&](auto&& evaluated_arg) {
        using EvaluatedType = std::decay_t<decltype(evaluated_arg)>;

        if (node.type == "i") 
        {
            if constexpr (std::is_arithmetic_v<EvaluatedType>) {
                concrete_decl = std::make_shared<Numeric<int>>(node.name, static_cast<int>(evaluated_arg));
            } else {
                throw std::runtime_error("Type Error: Cannot initialize int variable '" + node.name + "' with a non-numeric value.");
            }
        } 
        else if (node.type == "d") 
        {
            if constexpr (std::is_arithmetic_v<EvaluatedType>) {
                concrete_decl = std::make_shared<Numeric<double>>(node.name, static_cast<double>(evaluated_arg));
            } 
            else 
            {
                throw std::runtime_error("Type Error: Cannot initialize double variable '" + node.name + "' with a non-numeric value.");
            }
        } 
        else if (node.type == "f") 
        {
            if constexpr (std::is_arithmetic_v<EvaluatedType>) 
            {
                concrete_decl = std::make_shared<Numeric<float>>(node.name, static_cast<float>(evaluated_arg));
            } 
            else 
            {
                throw std::runtime_error("Type Error: Cannot initialize float variable '" + node.name + "' with a non-numeric value.");
            }
        }
        else if (node.type == "s")
        {
            if constexpr (std::is_same_v<EvaluatedType, std::string>) 
            {
                concrete_decl = std::make_shared<StringDeclaration>(node.name, evaluated_arg);
            } 
            else 
            {
                throw std::runtime_error("Type Error: Cannot initialize string variable '" + node.name + "' with a numeric value.");
            }
        }
    }, raw_value);

    if (!concrete_decl)
    {
        throw std::runtime_error("Error: Could not declare variable of type: " + node.type);
    }

    scope.define(node.name, concrete_decl);

    // DUBUG
    //
    std::cout << "[DECLARATION] " << concrete_decl->getName() << " <-- ";
    PrintVisitor print_visitor {};
    concrete_decl->accept(print_visitor);
    std::cout << std::endl;
}

void PrintVisitor::visit(const NumericDeclaration& num_decl) 
{
    if (num_decl.isFloatingPoint()) 
    {
        std::cout << "(floating point) " << num_decl.asDouble();
    } else 
    {
        std::cout << "(integer) " << num_decl.asInteger();
    }
}

void PrintVisitor::visit(const StringDeclaration& str_decl) 
{
    std::cout << "(string) '" << str_decl.getString() << "'";
}

RuntimeValue ExpressionEvaluator::evaluate(const Expression& expr) 
{
    expr.accept(*this);
    return last_evaluated_value;
}

void ExpressionEvaluator::visit(const LiteralExpression& expr)  
{
    if (expr.value.front() == '"' && expr.value.back() == '"')
    {
        last_evaluated_value = expr.value.substr(1, expr.value.length() - 2);
    }
    else
    {
        last_evaluated_value = std::stod(expr.value);   
    }
}

void ExpressionEvaluator::visit(const VariableExpression& expr) 
{
    auto decl = scope.lookup(expr.name);
    auto val_decl = std::dynamic_pointer_cast<ValueDeclaration>(decl);
    if (!val_decl) 
    {
        throw std::runtime_error("Error: " + expr.name + " does not elicit a value.");
    }
    
    last_evaluated_value = val_decl->getValue();
}