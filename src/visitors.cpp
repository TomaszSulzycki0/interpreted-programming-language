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
    if ( !existing_var )
    {
        throw std::runtime_error("Error: Variable '" + std::string( node.name ) + "' is undefined.");
    }

    auto target_var = std::dynamic_pointer_cast<ValueDeclaration>(existing_var);
    if (!target_var) 
    {
        throw std::runtime_error("Runtime Error: " + std::string( node.name ) + " is not a mutable variable.");
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
    if ( scope.lookup( node.name ))
    {
        throw std::runtime_error("Error: Variable '" + std::string( node.name ) + "' redefinition.");
    }

    RuntimeValue raw_value; 
    if ( node.initializer ) 
    {
        ExpressionEvaluator evaluator {scope};
        raw_value = evaluator.evaluate(*node.initializer);
    }
    else 
    {
        if (node.type == "int") raw_value = 0;
        else if (node.type == "ddouble" || node.type == "float") raw_value = 0.0;
        else if (node.type == "string") raw_value = "";
        else if (node.type == "bool") raw_value = 0;
    }

    std::shared_ptr<Declaration> concrete_decl = nullptr;

    std::visit([&](auto&& evaluated_arg) {
        using EvaluatedType = std::decay_t<decltype(evaluated_arg)>;

        if ( node.type == "int" ) 
        {
            if constexpr ( std::is_arithmetic_v<EvaluatedType> ) {
                concrete_decl = std::make_shared<Numeric<int>>(node.name, static_cast<int>(evaluated_arg));
            } else {
                throw std::runtime_error("Type Error: Cannot initialize int variable '" + std::string( node.name ) + "' with a non-numeric value.");
            }
        } 
        else if ( node.type == "double" ) 
        {
            if constexpr ( std::is_arithmetic_v<EvaluatedType> ) {
                concrete_decl = std::make_shared<Numeric<double>>(node.name, static_cast<double>(evaluated_arg));
            } 
            else 
            {
                throw std::runtime_error("Type Error: Cannot initialize double variable '" + std::string( node.name ) + "' with a non-numeric value.");
            }
        } 
        else if ( node.type == "float" ) 
        {
            if constexpr ( std::is_arithmetic_v<EvaluatedType> )  
            {
                concrete_decl = std::make_shared<Numeric<float>>(node.name, static_cast<float>(evaluated_arg));
            } 
            else 
            {
                throw std::runtime_error("Type Error: Cannot initialize float variable '" + std::string( node.name ) + "' with a non-numeric value.");
            }
        }
        else if ( node.type == "string" )
        {
            if constexpr (std::is_same_v<EvaluatedType, std::string>) 
            {
                concrete_decl = std::make_shared<StringDeclaration>(node.name, evaluated_arg);
            } 
            else 
            {
                throw std::runtime_error("Type Error: Cannot initialize string variable '" + std::string( node.name ) + "' with a numeric value.");
            }
        }
        else if ( node.type == "bool" )
        {
            if constexpr (std::is_arithmetic_v<EvaluatedType>) 
            {
                concrete_decl = std::make_shared<Numeric<bool>>(node.name, static_cast<bool>(evaluated_arg));
            } 
            else 
            {
                throw std::runtime_error("Type Error: Cannot initialize string variable '" + std::string( node.name ) + "' with a numeric value.");
            }
        }
    }, raw_value);

    if ( !concrete_decl )
    {
        throw std::runtime_error("Error: Could not declare variable of type: " + std::string( node.type ));
    }

    scope.define(node.name, concrete_decl);

    // DUBUG
    //
    std::cout << "[DECLARATION] " << concrete_decl->getName() << " <-- ";
    PrintVisitor print_visitor {};
    concrete_decl->accept(print_visitor);
    std::cout << std::endl;
}

void NodeMaker::visit(const FunctionDeclarationNode& node)
{
    std::shared_ptr<Declaration> concrete_decl = nullptr;
    
    if ( scope.lookup( node.name ))
    {
        throw std::runtime_error("Error: Function '" + std::string( node.name ) + "' redefinition.");
    }

    // Function local scope
    std::shared_ptr<Scope> fn_scope { std::make_shared<Scope>(scope) };
    
    NodeMaker fn_arg_maker { *fn_scope };

    // "Declare" the argument variables in the new scope
    for ( const auto& arg_decl : node.arg_nodes )
    {
        fn_arg_maker.visit(*arg_decl);
    }

    concrete_decl = std::make_shared<FunctionDeclaration>(  std::string( node.name ),
                                                            std::move( fn_scope ),
                                                            node.arg_nodes.size(),
                                                            std::move( node.body_nodes ) );
    
    if ( !concrete_decl )
    {
        throw std::runtime_error("Error: Could not declare function");
    }
    
    scope.define(node.name, concrete_decl);
}

void NodeMaker::visit(const FunctionCallNode& node)
{

}

void PrintVisitor::visit(const NumericDeclaration& num_decl) 
{
    if ( num_decl.isFloatingPoint() ) 
    {
        std::cout << "(floating point) " << num_decl.asDouble();
    } 
    else  
    {
        if ( num_decl.isBool() )
        {
            std::cout << "(bool) " << std::boolalpha << num_decl.asBool();
        }
        else
        {        
            std::cout << "(integer) " << num_decl.asInteger();
        }   
    }
}

void PrintVisitor::visit(const StringDeclaration& str_decl) 
{
    std::cout << "(string) '" << str_decl.getString() << "'";
}

void PrintVisitor::visit(const FunctionDeclaration& fn_decl) 
{
    
}

RuntimeValue ExpressionEvaluator::evaluate(const Expression& expr) 
{
    expr.accept(*this);
    return last_evaluated_value;
}

template <typename Op>
RuntimeValue evaluateBinaryOp(const RuntimeValue& v_l, const RuntimeValue& v_r, Op operation) 
{
    return std::visit([&](auto&& unpacked_left, auto&& unpacked_right) -> RuntimeValue {
        using TLeft = std::decay_t<decltype(unpacked_left)>;
        using TRight = std::decay_t<decltype(unpacked_right)>;

        if constexpr ( std::is_arithmetic_v<TLeft> && std::is_arithmetic_v<TRight> ) {
            return RuntimeValue{ operation( unpacked_left, unpacked_right ) };
        } 
        else if constexpr ( std::same_as<std::string, TLeft> && std::same_as<std::string, TRight> && std::same_as<std::plus<>, Op> ) {
            return RuntimeValue{ operation( unpacked_left, unpacked_right ) };
        }
        else 
        {
            throw std::runtime_error("Error: Unsupported operand types for this operation.");
        }
    }, v_l, v_r);
}

RuntimeValue ExpressionEvaluator::resolveOperator(const RuntimeValue& v_left, const RuntimeValue& v_right, const std::string& op) const
{
    static const std::map<std::string, std::function<RuntimeValue(const RuntimeValue&, const RuntimeValue&)>> operator_map = {
        { "+", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::plus<>{}); } },
        { "-", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::minus<>{}); } },
        { "*", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::multiplies<>{}); } },
        { "/", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::divides<>{}); } },
        { "==", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::equal_to<>{}); } },
        { ">=", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::greater_equal<>{}); } },
        { "<=", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::less_equal<>{}); } },
        { ">", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::greater<>{}); } },
        { "<", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::less<>{}); } },
        { "and", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::logical_and<>{}); } },
        { "or", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::logical_or<>{}); } }
    };

    auto it = operator_map.find(op);
    if ( it != operator_map.end() ) 
    {
        return it->second( v_left, v_right );
    }
    else
    {
        throw std::runtime_error("Error: Unknown operator " + op);
    }
    
}

void ExpressionEvaluator::visit(const BinaryExpression& expr)
{
    RuntimeValue eval_left = evaluate(*expr.expr_left);
    RuntimeValue eval_right = evaluate(*expr.expr_right);

    last_evaluated_value = resolveOperator(eval_left, eval_right, expr.expr_operator);
}

void ExpressionEvaluator::visit(const LiteralExpression& expr)  
{
    if ( expr.value.empty() )
    {
        last_evaluated_value = "";
    }
    else if ( expr.value.front() == '"' && expr.value.back() == '"' )
    {
        last_evaluated_value = expr.value.substr(1, expr.value.length() - 2);
    }
    else if ( expr.value == "true" )
    {
        last_evaluated_value = true;
    }
    else if ( expr.value == "false" )
    {
        last_evaluated_value = false;
    }
    else
    {
        last_evaluated_value = std::stod(expr.value);   
    }
}

void ExpressionEvaluator::visit(const VariableExpression& expr) 
{
    const std::string var_name = expr.name;

    auto decl = scope.lookup(var_name);
    if ( !decl )
    {
        throw std::runtime_error("Error: Variable '" + std::string( var_name ) + "' is undefined.");
    }

    auto val_decl = std::dynamic_pointer_cast<ValueDeclaration>(decl);
    if ( !val_decl ) 
    {
        throw std::runtime_error("Error: " + var_name + " does not elicit a value.");
    }
    
    last_evaluated_value = val_decl->getValue();
}

void ExpressionEvaluator::visit(const UnaryExpression& expr)
{
    RuntimeValue eval_child = evaluate(*expr.child);
    
    if ( expr.unary_op == "-")
    {
        last_evaluated_value = std::visit([&](auto&& unpacked_val) -> RuntimeValue {
            using T = std::decay_t<decltype(unpacked_val)>;
            if constexpr ( std::is_arithmetic_v<T> )
            {
                return -unpacked_val;
            }
            else
            {
                throw std::runtime_error("Error: Unsupported operand type for this operation.");
            }

        }, eval_child);
    }
    else if ( expr.unary_op == "!")
    {
        last_evaluated_value = std::visit([&](auto&& unpacked_val) -> RuntimeValue {
            using T = std::decay_t<decltype(unpacked_val)>;
            if constexpr ( std::is_arithmetic_v<T> )
            {
                return !unpacked_val;
            }
            else
            {
                throw std::runtime_error("Error: Unsupported operand type for this operation.");
            }
        }, eval_child);
    }
}

void ExpressionEvaluator::visit(const FunctionCallExpression& expr)
{

}