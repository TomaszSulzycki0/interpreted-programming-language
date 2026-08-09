#include"visitors.hpp"
#include"expression.hpp"
#include"scope.hpp"
#include"numericVariable.hpp"
#include"stringVariable.hpp"
#include"AbstractNodes.hpp"

void NodeMaker::visit(const AssignmentNode& node)
{
    ExpressionEvaluator evaluator {scope};
    RuntimeValue new_value = evaluator.evaluate(*node.value_expr);
    
    auto existing_var = scope->lookup(node.name);
    if ( !existing_var )
    {
        throw std::runtime_error("Error: '" + std::string( node.name ) + "' is undefined.");
    }

    auto target_var = std::dynamic_pointer_cast<ValueDeclaration>(existing_var);
    if (!target_var) 
    {
        throw std::runtime_error("Error: " + std::string( node.name ) + " is not a mutable variable.");
    }

    std::visit([&](auto&&) {
        target_var->setValue(new_value);
    }, new_value);

    // DUBUG
    //
    std::cout << "[ASSIGNMENT] " << existing_var->getName() << " ";
    PrintVisitor print_visitor {};
    existing_var->accept(print_visitor);
    std::cout << std::endl;
}

void NodeMaker::visit(const DeclarationNode& node)
{
    if ( scope->lookupLocal( node.name ))
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
                throw std::runtime_error("Error: Cannot initialize int variable '" + std::string( node.name ) + "' with a non-numeric value.");
            }
        } 
        else if ( node.type == "double" ) 
        {
            if constexpr ( std::is_arithmetic_v<EvaluatedType> ) {
                concrete_decl = std::make_shared<Numeric<double>>(node.name, static_cast<double>(evaluated_arg));
            } 
            else 
            {
                throw std::runtime_error("Error: Cannot initialize double variable '" + std::string( node.name ) + "' with a non-numeric value.");
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
                throw std::runtime_error("Error: Cannot initialize float variable '" + std::string( node.name ) + "' with a non-numeric value.");
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
                throw std::runtime_error("Error: Cannot initialize string variable '" + std::string( node.name ) + "' with a numeric value.");
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
                throw std::runtime_error("Error: Cannot initialize string variable '" + std::string( node.name ) + "' with a numeric value.");
            }
        }
    }, raw_value);

    if ( !concrete_decl )
    {
        throw std::runtime_error("Error: Could not declare variable of type: " + std::string( node.type ));
    }

    scope->define(node.name, concrete_decl);

    // DUBUG
    //
    std::cout << "[DECLARATION] " << concrete_decl->getName() << " ";
    PrintVisitor print_visitor {};
    concrete_decl->accept(print_visitor);
    std::cout << std::endl;
}

void NodeMaker::visit(const FunctionDeclarationNode& node)
{
    std::shared_ptr<Declaration> concrete_decl = nullptr;
    
    if ( scope->lookupLocal( node.name ))
    {
        throw std::runtime_error("Error: Function '" + std::string( node.name ) + "' redefinition.");
    }

    // Function local scope
    std::shared_ptr<Scope> fn_scope { std::make_shared<Scope>(scope) };
    
    NodeMaker fn_arg_maker { fn_scope };

    std::vector<std::string> fn_arg_names;

    for ( const auto& arg_decl : node.arg_nodes )
    {
        // "Declare" the argument variable in the new scope
        fn_arg_maker.visit(*arg_decl);
        fn_arg_names.push_back( arg_decl->name );
    }

    // AST Nodes in function body are owned by FunctionDeclarationNode
    // FunctionDeclaration stores raw pointers
    std::vector<ASTNode*> raw_body;
    for (const auto& ptr : node.body_nodes) 
    {
        raw_body.push_back(ptr.get());
    }

    concrete_decl = std::make_shared<FunctionDeclaration>(  std::string( node.name ),
                                                            node.arg_nodes.size(),
                                                            fn_arg_names,
                                                            node.return_type,
                                                            std::move( fn_scope ),
                                                            raw_body,
                                                            node.return_expr.get() );
    
    if ( !concrete_decl )
    {
        throw std::runtime_error("Error: Could not declare function");
    }
    
    scope->define(node.name, concrete_decl);

    // DEBUG
    //
    std::cout << "[FUNCTION] " << concrete_decl->getName();
    PrintVisitor print_visitor {};
    concrete_decl->accept(print_visitor);
    std::cout << std::endl;
}

void NodeMaker::visit(const FunctionCallNode& node)
{
    ExpressionEvaluator fn_eval { scope };

    fn_eval.visit( *(node.expr) );
}

void NodeMaker::visit(const IfNode& node)
{
    ExpressionEvaluator if_condition_evaluator { scope };

    RuntimeValue if_eval = if_condition_evaluator.evaluate( *node.condition_expr ); 

    std::visit([&](auto&& unpacked) {
        using T = std::decay_t<decltype(unpacked)>;

        if constexpr ( std::is_convertible_v<T, bool> )
        {
            if ( !unpacked )
            {
                return;
            }
        }
        else if constexpr ( std::same_as<T, std::string> )
        {
            if ( unpacked.empty() )
            {
                return;
            }
        }
        else
        {
            throw std::runtime_error("Error: Expression not convertible to bool.");
        }

    }, if_eval);

    std::shared_ptr<Scope> if_scope { std::make_shared<Scope>( scope ) };

    NodeMaker if_body_exec { if_scope };

    for( const auto& nd : node.body_nodes )
    {
        nd->accept( if_body_exec );
    }

}

void NodeMaker::visit(const WhileNode& node)
{
    ExpressionEvaluator while_condition_evaluator { scope };

    RuntimeValue while_eval = while_condition_evaluator.evaluate( *node.condition_expr ); 

    std::visit([&](auto&& unpacked) {
        using T = std::decay_t<decltype(unpacked)>;

        if constexpr ( std::is_convertible_v<T, bool> )
        {
            if ( !unpacked )
            {
                return;
            }
        }
        else if constexpr ( std::same_as<T, std::string> )
        {
            if ( unpacked.empty() )
            {
                return;
            }
        }
        else
        {
            throw std::runtime_error("Error: Expression not convertible to bool.");
        }

    }, while_eval);

    std::shared_ptr<Scope> while_scope { std::make_shared<Scope>( scope ) };

    NodeMaker while_body_exec { while_scope };

    bool while_run = true;

    while( while_run )
    {

        for( const auto& nd : node.body_nodes )
        {
            nd->accept( while_body_exec );
        }
    
        while_eval = while_condition_evaluator.evaluate( *node.condition_expr );    
        
        std::visit([&](auto&& unpacked) {
        using T = std::decay_t<decltype(unpacked)>;

        if constexpr ( std::is_convertible_v<T, bool> )
        {
            if ( !unpacked )
            {
                while_run = false;
            }
        }
        else if constexpr ( std::same_as<T, std::string> )
        {
            if ( unpacked.empty() )
            {
                while_run = false;
            }
        }
        else
        {
            throw std::runtime_error("Error: Expression not convertible to bool.");
        }

        }, while_eval);

    }

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
    std::cout << "(";
    for( auto i {0uz}; i < fn_decl.getNumArgs(); ++i  )
    {
        std::cout << fn_decl.getArgNames()[i];
        if ( i < fn_decl.getNumArgs() - 1 )
        {
            std::cout << ", ";
        }
    }

    std::cout << ")";
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

    auto decl = scope->lookup(var_name);
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
    auto existing_fn = scope->lookup(expr.name);
    if ( !existing_fn )
    {
        throw std::runtime_error("Error: Function '" + std::string( expr.name ) + "' is undefined.");
    }

    auto target_fn = std::dynamic_pointer_cast<FunctionDeclaration>(existing_fn);
    if ( !target_fn ) 
    {
        throw std::runtime_error("Error: " + std::string( expr.name ) + " is not a callable object.");
    }

    if ( target_fn->getNumArgs() != expr.args.size() ) 
    {
        throw std::runtime_error("Error: Invalid number of arguments provided. Expected " + std::to_string( target_fn->getNumArgs() ) + "." );
    }

    NodeMaker node_exec { target_fn->scope };
    ExpressionEvaluator ret_val_eval { target_fn->scope };

    for( std::size_t i {}; i < target_fn->getNumArgs(); ++i )
    {

        // Evaluate argument in caller scope
        ExpressionEvaluator evaluator {scope};
        RuntimeValue arg_value = evaluator.evaluate( *( expr.args[i]->clone() ) );
        
        // Assign evaluated value to argument in function local scope
        auto existing_var = target_fn->scope->lookup( target_fn->getArgNames()[i] );
        if ( !existing_var )
        {
            throw std::runtime_error("Error: Variable '" + std::string( target_fn->getArgNames()[i] ) + "' is undefined.");
        }

        auto target_var = std::dynamic_pointer_cast<ValueDeclaration>(existing_var);
        if (!target_var) 
        {
            throw std::runtime_error("Error: " + std::string( target_fn->getArgNames()[i] ) + " is not a mutable variable.");
        }

        std::visit([&](auto&&) {
            target_var->setValue(arg_value);
        }, arg_value);
    }

    for ( const auto& nd : target_fn->body_nodes )
    {
        nd->accept(node_exec);
    }

    // if non void
    if ( target_fn->return_expr )
    {
        last_evaluated_value = ret_val_eval.evaluate( *(target_fn->return_expr) );
    }
}

void NodeTypeChecker::visit(const AssignmentNode& node)
{
    std::string var_type = scope->lookup( node.name );

    if ( var_type.empty() )
    {
        throw std::runtime_error("Type error: Variable '" + node.name + "' is undefined.");
    }

    ExpressionTypeEvaluator type_evaluator { scope };

    std::string expr_type = type_evaluator.evaluateType( *node.value_expr );

    if ( expr_type.empty() )
    {
        throw std::runtime_error("Type error: Unable to evaluate expression type.");
    }

    if ( expr_type != var_type )
    {
        throw std::runtime_error("Type error: Type '" + var_type + "' of declaration '" + node.name + "' is incompatible with '" + expr_type + "'.");
    }
    
}

void NodeTypeChecker::visit(const DeclarationNode& node)
{
    const auto& declaration_type = node.type;

    if ( !( scope->lookupLocal( node.name ).empty() ) )
    {
        throw std::runtime_error("Error: Variable '" + node.name + "' was already declared in this scope.");
    }

    if( !node.initializer )
    {
        return;
    }

    ExpressionTypeEvaluator type_evaluator { scope };

    std::string expr_type = type_evaluator.evaluateType( *node.initializer );

    if ( expr_type.empty() )
    {
        throw std::runtime_error("Type error: Unable to evaluate expression type.");
    }

    // Implicit casting here

    if ( expr_type != declaration_type )
    {
        throw std::runtime_error("Type error: Declared type '" + declaration_type + "' of declaration '" + node.name + "' is incompatible with '" + expr_type + "'.");
    }

    scope->define( node.name, declaration_type );

}

void NodeTypeChecker::visit(const FunctionDeclarationNode& node)
{
    // TODO: Save argument types, number of arguments
    // TODO: Run type checker inside function body

    scope->define( node.name, node.return_type );
}

void NodeTypeChecker::visit(const FunctionCallNode& node)
{
    std::string fn_type = scope->lookup( node.name );

    if ( fn_type.empty() )
    {
        throw std::runtime_error("Type error: Function '" + node.name + "' is undefined.");
    }

    // TODO: Warning about ignoring return value for non-void functions
}

std::string ExpressionTypeEvaluator::evaluateType(const Expression& expr)
{
    expr.accept(*this);
    return last_evaluated_type;
}

void ExpressionTypeEvaluator::visit(const LiteralExpression& expr)
{
    last_evaluated_type = expr.tp;
}

void ExpressionTypeEvaluator::visit(const VariableExpression& expr)
{
    std::string var_type = scope->lookup( expr.name );

    if ( var_type.empty() )
    {
        throw std::runtime_error("Type error: Variable '" + expr.name + "' is undefined.");
    }

    last_evaluated_type = var_type;
}

void ExpressionTypeEvaluator::visit(const BinaryExpression& expr)
{
    // TODO: Checking operand-operator compatibility
    // TODO: Arithmetic upcasting

    std::string left_type = this->evaluateType( *(expr.expr_left) );
    std::string right_type = this->evaluateType( *(expr.expr_right) );

    if ( left_type != right_type )
    {
        throw std::runtime_error("Type error: Implicit type conversion not allowed. Evaluated types: '" + left_type + "', '" + right_type + "'.");
    }

    last_evaluated_type = left_type;
}

void ExpressionTypeEvaluator::visit(const UnaryExpression& expr)
{
    last_evaluated_type = this->evaluateType( *(expr.child) );
}

void ExpressionTypeEvaluator::visit(const FunctionCallExpression& expr)
{
    std::string fn_ret_type = scope->lookup( expr.name );

    if ( fn_ret_type.empty() )
    {
        throw std::runtime_error("Type error: Function '" + expr.name + "' is undefined.");
    }

    last_evaluated_type = fn_ret_type;
}



