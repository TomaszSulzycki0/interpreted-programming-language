#include<iostream>

#include"builder.hpp"
#include"parser.hpp"
#include"visitorsBase.hpp"
#include"expression.hpp"
#include"scope.hpp"
#include"numericVariable.hpp"
#include"stringVariable.hpp"
#include"abstractNodes.hpp"
#include"debugMacros.hpp"
#include"implementedType.hpp"

void Builder::buildProgram(const std::vector<std::unique_ptr<ASTNode>>& ast) 
{
    NodeMaker node_visitor {scope};

    for (const auto& node_ptr : ast) 
    {        
        node_ptr->accept(node_visitor);
    }
}


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

    #ifdef DEBUG_EXEC
        std::cout << "[ASSIGNMENT] " << existing_var->getName() << " ";
        PrintVisitor print_visitor {};
        existing_var->accept(print_visitor);
        std::cout << std::endl;
    #endif
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
        if (node.type == ImplementedType::_int) raw_value = 0;
        else if (node.type == ImplementedType::_double || node.type == ImplementedType::_float) raw_value = 0.0;
        else if (node.type == ImplementedType::_string) raw_value = "";
        else if (node.type == ImplementedType::_bool) raw_value = 0;
    }

    std::shared_ptr<Declaration> concrete_decl = nullptr;

    std::visit([&](auto&& evaluated_arg) {
        using EvaluatedType = std::decay_t<decltype(evaluated_arg)>;

        if ( node.type == ImplementedType::_int ) 
        {
            if constexpr ( std::is_arithmetic_v<EvaluatedType> ) {
                concrete_decl = std::make_shared<Numeric<int>>(node.name, static_cast<int>(evaluated_arg));
            } else {
                throw std::runtime_error("Error: Cannot initialize int variable '" + std::string( node.name ) + "' with a non-numeric value.");
            }
        } 
        else if ( node.type == ImplementedType::_double ) 
        {
            if constexpr ( std::is_arithmetic_v<EvaluatedType> ) {
                concrete_decl = std::make_shared<Numeric<double>>(node.name, static_cast<double>(evaluated_arg));
            } 
            else 
            {
                throw std::runtime_error("Error: Cannot initialize double variable '" + std::string( node.name ) + "' with a non-numeric value.");
            }
        } 
        else if ( node.type == ImplementedType::_float ) 
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
        else if ( node.type == ImplementedType::_string )
        {
            if constexpr (std::is_same_v<EvaluatedType, std::string>) 
            {
                concrete_decl = std::make_shared<StringDeclaration>(node.name, evaluated_arg);
            } 
            else if constexpr (std::is_arithmetic_v<EvaluatedType>)
            {
                concrete_decl = std::make_shared<StringDeclaration>(node.name, std::format("{}", evaluated_arg));
            }
            else
            {
                throw std::runtime_error("Error: Cannot initialize string variable '" + node.name + "'.");
            }
        }
        else if ( node.type == ImplementedType::_bool )
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
        throw std::runtime_error("Error: Could not declare variable of type: " + node.type );
    }

    scope->define(node.name, concrete_decl);

    #ifdef DEBUG_EXEC
        std::cout << "[DECLARATION] " << concrete_decl->getName() << " ";
        PrintVisitor print_visitor {};
        concrete_decl->accept(print_visitor);
        std::cout << std::endl;
    #endif
}

void NodeMaker::visit(const FunctionDeclarationNode& node)
{
    std::shared_ptr<Declaration> concrete_decl = nullptr;
    
    if ( scope->lookupLocal( node.name ))
    {
        throw std::runtime_error("Error: Function '" + std::string( node.name ) + "' redefinition.");
    }

    std::vector<std::string> fn_arg_names;

    for ( const auto& arg_decl : node.arg_nodes )
    {
        fn_arg_names.push_back( arg_decl->name );
    }

    // AST Nodes in function body are owned by FunctionDeclarationNode
    // FunctionDeclaration stores raw pointers
    std::vector<ASTNode*> raw_body;
    for (const auto& ptr : node.body_nodes) 
    {
        raw_body.push_back(ptr.get());
    }

    // Same as above for arguments
    std::vector<DeclarationNode*> raw_args;
    for (const auto& ptr : node.arg_nodes) 
    {
        raw_args.push_back(ptr.get());
    }

    concrete_decl = std::make_shared<FunctionDeclaration>(  std::string( node.name ),
                                                            node.arg_nodes.size(),
                                                            fn_arg_names,
                                                            node.return_type,
                                                            raw_body,
                                                            raw_args,
                                                            scope );
    
    if ( !concrete_decl )
    {
        throw std::runtime_error("Error: Could not declare function");
    }
    
    scope->define(node.name, concrete_decl);

    #ifdef DEBUG_EXEC
        std::cout << "[FUNCTION DECLARATION] " << concrete_decl->getName();
        PrintVisitor print_visitor {};
        concrete_decl->accept(print_visitor);
        std::cout << std::endl;
    #endif
}

void NodeMaker::visit(const FunctionCallNode& node)
{
    #ifdef DEBUG_EXEC
        std::cout << "[VOID FUNCTION CALL] " << node.name << std::endl;
    #endif

    ExpressionEvaluator fn_eval { scope };

    fn_eval.visit( *(node.expr) );
}

void NodeMaker::visit(const IfNode& node)
{
    ExpressionEvaluator if_condition_evaluator { scope };

    RuntimeValue if_eval = if_condition_evaluator.evaluate( *node.condition_expr );

    bool should_execute = true;

    std::visit([&](auto&& unpacked) {
        using T = std::decay_t<decltype(unpacked)>;

        if constexpr ( std::is_convertible_v<T, bool> )
        {
            if ( !unpacked )
            {
                should_execute = false;
            }
        }
        else if constexpr ( std::same_as<T, std::string> )
        {
            if ( unpacked.empty() )
            {
                should_execute = false;
            }
        }
        else
        {
            throw std::runtime_error("Error: Expression not convertible to bool.");
        }

    }, if_eval);

    if ( !should_execute)
    {
        if ( node.else_nd )
        {
            this->visit( *(node.else_nd) );
        }
        return;
    }

    #ifdef DEBUG_EXEC
        std::cout << "[ENTER IF STATEMENT]" << std::endl;
    #endif

    std::shared_ptr<Scope> if_scope { std::make_shared<Scope>( scope ) };

    NodeMaker if_body_exec { if_scope };

    for( const auto& nd : node.body_nodes )
    {
        nd->accept( if_body_exec );
        if ( if_body_exec.reached_return_statement )
        {
            this->reached_return_statement = true;
            this->return_expression = std::move( if_body_exec.return_expression );
            this->return_expression_scope = std::move( if_body_exec.return_expression_scope );
            break;
        }
    }

    #ifdef DEBUG_EXEC
        std::cout << "[EXIT IF STATEMENT]" << std::endl;
    #endif

}

void NodeMaker::visit(const ElseNode& node)
{
    #ifdef DEBUG_EXEC
        std::cout << "[ENTER ELSE STATEMENT]" << std::endl;
    #endif

    std::shared_ptr<Scope> else_scope { std::make_shared<Scope>( scope ) };

    NodeMaker else_body_exec { else_scope };

    for( const auto& nd : node.body_nodes )
    {
        nd->accept( else_body_exec );
        if ( else_body_exec.reached_return_statement )
        {
            this->reached_return_statement = true;
            this->return_expression = std::move( else_body_exec.return_expression );
            this->return_expression_scope = std::move( else_body_exec.return_expression_scope );
            break;
        }
    }

    #ifdef DEBUG_EXEC
        std::cout << "[EXIT ELSE STATEMENT]" << std::endl;
    #endif
}

void NodeMaker::visit(const WhileNode& node)
{
    ExpressionEvaluator while_condition_evaluator { scope };

    RuntimeValue while_eval = while_condition_evaluator.evaluate( *node.condition_expr ); 

    bool should_execute = true;

    std::visit([&](auto&& unpacked) {
        using T = std::decay_t<decltype(unpacked)>;

        if constexpr ( std::is_convertible_v<T, bool> )
        {
            if ( !unpacked )
            {
                should_execute = false;
            }
        }
        else if constexpr ( std::same_as<T, std::string> )
        {
            if ( unpacked.empty() )
            {
                should_execute = false;
            }
        }
        else
        {
            throw std::runtime_error("Error: Expression not convertible to bool.");
        }

    }, while_eval);

    if ( !should_execute )
    {
        return;
    }

    #ifdef DEBUG_EXEC
        std::cout << "[ENTER WHILE LOOP]" << std::endl;
    #endif

    while( should_execute )
    {
        std::shared_ptr<Scope> while_scope { std::make_shared<Scope>( scope ) };

        NodeMaker while_body_exec { while_scope };

        for( const auto& nd : node.body_nodes )
        {
            nd->accept( while_body_exec );
            if ( while_body_exec.reached_return_statement )
            {
                this->reached_return_statement = true;
                this->return_expression = std::move( while_body_exec.return_expression );
                this->return_expression_scope = std::move( while_body_exec.return_expression_scope );
                break;
            }
        }

        if ( reached_return_statement )
        {
            break;
        }

        while_eval = while_condition_evaluator.evaluate( *node.condition_expr );    
        
        std::visit([&](auto&& unpacked) {
        using T = std::decay_t<decltype(unpacked)>;

        if constexpr ( std::is_convertible_v<T, bool> )
        {
            if ( !unpacked )
            {
                should_execute = false;
            }
        }
        else if constexpr ( std::same_as<T, std::string> )
        {
            if ( unpacked.empty() )
            {
                should_execute = false;
            }
        }
        else
        {
            throw std::runtime_error("Error: Expression not convertible to bool.");
        }

        }, while_eval);

    }

    #ifdef DEBUG_EXEC
        std::cout << "[EXIT WHILE LOOP]" << std::endl;
    #endif

}

void NodeMaker::visit(const ReturnNode& node)
{
    #ifdef DEBUG_EXEC
        std::cout << "[CONTROL REACHED RETURN]" << std::endl;
    #endif

    this->reached_return_statement = true;
    if ( node.ret_expr )
    {
        this->return_expression = node.ret_expr->clone();
        this->return_expression_scope = this->scope;
    }
    else
    {
        this->return_expression = nullptr;
    }
}

void NodeMaker::visit(const EmbeddedPrintFunctionNode&)
{
    #ifdef DEBUG_EXEC
        std::cout << "[BUILT IN PRINT FUNCTION CALL]" << std::endl;
    #endif

    auto input = this->scope->lookupLocal("data");

    if ( !input )
    {
        throw std::runtime_error("Error: Unexpected error while trying to pass input to print function.");
    }

    auto input_str = std::dynamic_pointer_cast<StringDeclaration>(input);

    if ( !input_str ) 
    {
        throw std::runtime_error("Error: " + std::string( input->getName() ) + " is not a printable object.");
    }

    std::cout << input_str->getString();
}

void NodeMaker::visit(const EmbeddedCastFunctionNode& node)
{
    #ifdef DEBUG_EXEC
        std::cout << "[BUILT IN CAST FUNCTION CALL]" << std::endl;
    #endif

    auto input = this->scope->lookupLocal("data");

    if ( !input )
    {
        throw std::runtime_error("Error: Unexpected error while trying to pass input to print function.");
    }

    auto input_valued = std::dynamic_pointer_cast<ValueDeclaration>(input);

    if ( !input_valued ) 
    {
        throw std::runtime_error("Error: " + std::string( input->getName() ) + " does not elicit a value.");
    }

    // A casting function's return node returns a VariableExpression holding the variable "ret"
    // here we have to declare "ret" as the input cast to the appropriate type
    std::shared_ptr<DeclarationNode> temp_returnable { nullptr };
    std::unique_ptr<LiteralExpression> post_cast { nullptr };
    if ( node.tp == ImplementedType::_bool )
    {
        auto value = std::visit( [&](auto&& unpacked) -> bool
        {
            using T = std::decay_t<decltype(unpacked)>;

            if constexpr ( std::is_arithmetic_v<T> )
            {
                return static_cast<bool>(unpacked);
            }
            return false;
        }, input_valued->getValue());

        post_cast = std::make_unique<LiteralExpression>(std::to_string(value), ImplementedType::_bool);
    }
    else if ( node.tp == ImplementedType::_int )
    {
        auto value = std::visit( [&](auto&& unpacked) -> int
        {
            using T = std::decay_t<decltype(unpacked)>;

            if constexpr ( std::is_arithmetic_v<T> )
            {
                return static_cast<int>(unpacked);
            }
            return 0;
        }, input_valued->getValue());

        post_cast = std::make_unique<LiteralExpression>(std::to_string(value), ImplementedType::_int);
    }
    else if ( node.tp == ImplementedType::_float )
    {
        auto value = std::visit( [&](auto&& unpacked) -> float
        {
            using T = std::decay_t<decltype(unpacked)>;

            if constexpr ( std::is_arithmetic_v<T> )
            {
                return static_cast<float>(unpacked);
            }
            return 0;
        }, input_valued->getValue());

        post_cast = std::make_unique<LiteralExpression>(std::to_string(value), ImplementedType::_float);
    }
    else if ( node.tp == ImplementedType::_double )
    {
        auto value = std::visit( [&](auto&& unpacked) -> double
        {
            using T = std::decay_t<decltype(unpacked)>;

            if constexpr ( std::is_arithmetic_v<T> )
            {
                return static_cast<double>(unpacked);
            }
            return 0;
        }, input_valued->getValue());


        post_cast = std::make_unique<LiteralExpression>(std::to_string(value), ImplementedType::_double);
    }
    else if ( node.tp == ImplementedType::_string )
    {
        auto value = std::visit( [&](auto&& unpacked) -> std::string
        {
            using T = std::decay_t<decltype(unpacked)>;

            if constexpr ( std::is_arithmetic_v<T> )
            {
                return std::to_string(unpacked);
            }
            return "";
        }, input_valued->getValue());


        post_cast = std::make_unique<LiteralExpression>(value, ImplementedType::_string);
    }
    else
    {
        throw std::runtime_error("Error: Unable to cast type: '" + node.tp + "'.");
    }

    temp_returnable = std::make_shared<DeclarationNode>( node.tp, "ret", std::move( post_cast ));
    this->visit( *(temp_returnable) ); 
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

        if constexpr ( std::same_as<std::modulus<>, Op> ) 
        {
            if constexpr ( std::same_as<int, TLeft> && std::same_as<int, TRight> ) 
            {
                return RuntimeValue{ operation( unpacked_left, unpacked_right ) };
            }
            else 
            {
                throw std::runtime_error("Error: Unsupported operand types for this operation.");
            }
        }
       
        if constexpr ( std::is_arithmetic_v<TLeft> && std::is_arithmetic_v<TRight> && !std::same_as<std::modulus<>, Op> ) 
        {
            return RuntimeValue{ operation( unpacked_left, unpacked_right ) };
        } 
        else if constexpr ( std::same_as<std::string, TLeft> && std::same_as<std::string, TRight> && std::same_as<std::plus<>, Op> ) 
        {
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
        { "%", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::modulus<>{}); } },
        { "/", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::divides<>{}); } },
        { "==", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::equal_to<>{}); } },
        { "!=", [](auto& l, auto& r) { return evaluateBinaryOp(l, r, std::not_equal_to<>{}); } },
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
        int result_int = 0;

        const char* start = expr.value.data();
        const char* end = expr.value.data() + expr.value.size();

        auto [ptr, ec] = std::from_chars(start, end, result_int);

        if (ec == std::errc()) 
        {
            if (ptr == end) 
            {
                last_evaluated_value = result_int;
            } 
            else 
            {
                try
                {
                    last_evaluated_value = std::stod(expr.value);
                }
                catch(const std::exception& e)
                {
                    throw std::runtime_error("Error: Unable to parse numeric literal.");
                }  
            }
        } 
        else if (ec == std::errc::invalid_argument) 
        {
            throw std::runtime_error("Error: Unable to parse numeric literal.");
        } 
        else if (ec == std::errc::result_out_of_range) 
        {
            throw std::runtime_error("Error: Literal to large for int.");
        }
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
    #ifdef DEBUG_EXEC
        std::cout << "[EVALUATING FUNCTION CALL] "  << expr.name << std::endl;
    #endif

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

    std::shared_ptr<Scope> fn_scope = std::make_shared<Scope>( target_fn->declaration_scope );
    
    NodeMaker node_exec { fn_scope };

    for( std::size_t i {}; i < target_fn->getNumArgs(); ++i )
    {        
        // Make argument declarations
        target_fn->arg_nodes[i]->accept( node_exec );

        // Evaluate argument in caller scope
        ExpressionEvaluator evaluator {scope};
        RuntimeValue arg_value = evaluator.evaluate( *( expr.args[i]->clone() ) );
        
        // Assign evaluated value to argument in function local scope
        auto existing_var = fn_scope->lookup( target_fn->getArgNames()[i] );
        if ( !existing_var )
        {
            throw std::runtime_error("Error: Variable '" + std::string( target_fn->getArgNames()[i] ) + "' is undefined.");
        }

        auto target_var = std::dynamic_pointer_cast<ValueDeclaration>(existing_var);
        if (!target_var) 
        {
            throw std::runtime_error("Error: " + std::string( target_fn->getArgNames()[i] ) + " is not a mutable variable.");
        }

        target_var->setValue(arg_value);

        #ifdef DEBUG_EXEC
        std::cout << "[ARGUMENT ASSIGNMENT] "  << target_var->getName() << " = " << arg_value << std::endl;
        #endif    
    }

    for ( const auto& nd : target_fn->body_nodes )
    {
        nd->accept(node_exec);
        if ( node_exec.reached_return_statement )
        {
            Expression* ret_expr = node_exec.getReturnExpression();
            std::shared_ptr<Scope> ret_expr_scope = node_exec.getReturnExpressionScope();
            
            if ( ret_expr && ret_expr_scope )
            {
                ExpressionEvaluator ret_val_eval { ret_expr_scope };
                last_evaluated_value = ret_val_eval.evaluate( *(ret_expr) );
            }
            break;
        }
    }
}
