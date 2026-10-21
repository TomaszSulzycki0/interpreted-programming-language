#include"typeChecker.hpp"
#include"abstractNodes.hpp"
#include"scope.hpp"
#include"expression.hpp"
#include"parser.hpp"

void TypeChecker::run_check(const std::vector<std::unique_ptr<ASTNode>>& nodes)
{
    NodeTypeChecker node_visitor {scope};

    for ( const auto& node : nodes )
    {
        try
        {
            node->accept( node_visitor );
        }
        catch ( const std::exception& e )
        {
            std::cerr << e.what() << '\n';
            is_well_typed = false;
        }
    }
}

std::string NodeTypeChecker::getInverseTypeConversionRank(int rank)
{
    switch (rank) 
    {
        case 0:     return "bool";
        case 1:     return "int";
        case 2:     return "float";
        case 3:     return "double";
        default:    return "";
    }
}

void NodeTypeChecker::visit(const AssignmentNode& node)
{
    ExpressionTypeEvaluator type_evaluator { scope };

    std::string var_type = type_evaluator.evaluateType(*node.assignee);

    if ( var_type.empty() )
    {
        throw std::runtime_error("Error: Variable '" + node.assignee->name + "' is undefined.");
    }

    std::string expr_type = type_evaluator.evaluateType( *node.value_expr );

    if ( expr_type.empty() )
    {
        throw std::runtime_error("Type error: Unable to evaluate expression type.");
    }

    if ( expr_type != var_type )
    {
        checkTypeUpCasting( expr_type, var_type );
    }
    
}

void NodeTypeChecker::visit(const DeclarationNode& node)
{
    const auto& declaration_type = node.type;

    if ( scope->lookupLocal( node.name ).type != "" )
    {
        throw std::runtime_error("Error: Variable '" + node.name + "' was already declared in this scope.");
    }

    if( !node.initializer )
    {
        scope->define( node.name, VariableData(declaration_type) );
        return;
    }

    ExpressionTypeEvaluator type_evaluator { scope };

    std::string expr_type = type_evaluator.evaluateType( *node.initializer );

    if ( expr_type.empty() )
    {
        throw std::runtime_error("Type error: Unable to evaluate expression type.");
    }

    if ( expr_type != declaration_type )
    {
        checkTypeUpCasting( expr_type, declaration_type );
    }
        
    scope->define( node.name, VariableData(declaration_type) );
}

void NodeTypeChecker::visit(const FunctionDeclarationNode& node)
{
    if ( !(scope->lookup( node.name ).type.empty()) )
    {
        throw std::runtime_error("Error: Function '" + node.name + "' was already declared in this scope.");
    }

    std::shared_ptr<SemanticScope> fn_body_scope = std::make_shared<SemanticScope>( scope );
    NodeTypeChecker fn_body_checker { fn_body_scope };

    fn_body_checker.fn_return_type = node.return_type;

    std::vector<std::string> fn_arg_data {};

    for ( const auto& arg_node : node.arg_nodes )
    {
        fn_body_scope->define( arg_node->name,  VariableData(arg_node->type));    
        fn_arg_data.push_back( arg_node->type );

        if ( dynamic_cast<ObjectDeclarationNode*>( arg_node.get() ) )
        {
            fn_body_scope->defineObjectInstanceType( arg_node->name, arg_node->type );
        }
    }

    scope->storeFnArgTypes( node.name, fn_arg_data );
    scope->define( node.name,  VariableData("fn", node.return_type) );

    for ( const auto& bd_node : node.body_nodes )
    {    
        // We do not run the checker inside a built-in function
        if ( dynamic_cast<EmbeddedFunctionNode*>( bd_node.get() ) )
        {
            return;
        }
        bd_node->accept( fn_body_checker );   
    }

    if ( ( node.return_type != "void" ) && !( fn_body_checker.isReturnSafe ) )
    {
        throw std::runtime_error("Error: Missing return statement inside non-void function.");
    }

}

void NodeTypeChecker::visit(const FunctionCallNode& node)
{
    std::string fn_return_type = scope->lookup( node.name ).function_return_type;
    
    if ( fn_return_type.empty() )
    {
        throw std::runtime_error("Error: Function '" + node.name + "' is undefined.");
    }

    ExpressionTypeEvaluator type_evaluator { scope };
    
    [[maybe_unused]] std::string expr_type = type_evaluator.evaluateType( *node.expr );

}

void NodeTypeChecker::visit(const WhileNode& node)
{
    std::shared_ptr<SemanticScope> while_body_scope = std::make_shared<SemanticScope>( scope );
    NodeTypeChecker while_body_checker { while_body_scope };
    while_body_checker.fn_return_type = this->fn_return_type;

    for ( const auto& bd_node : node.body_nodes )
    {    
        bd_node->accept( while_body_checker );   
    }
}

void NodeTypeChecker::visit(const IfNode& node) 
{
    std::shared_ptr<SemanticScope> if_body_scope = std::make_shared<SemanticScope>( scope );
    NodeTypeChecker if_body_checker { if_body_scope };
    if_body_checker.fn_return_type = this->fn_return_type;

    for ( const auto& bd_node : node.body_nodes )
    {    
        bd_node->accept( if_body_checker );   
    }

    if ( node.else_nd )
    {
        this->visit( *(node.else_nd) );
    }

    // We check if both the "if" and "else" blocks have return statements.
    // Because the same NodeTypeChecker calls visit for ElseNode and IfNode
    // inside the visit(ElseNode) we change "this->isReturnSafe"  
    // so here we need to change it back to false in case the IfNode didnt have a return statement
    if ( !( this->isReturnSafe && if_body_checker.isReturnSafe ) )
    {
        this->isReturnSafe = false;
    }
}

void NodeTypeChecker::visit(const ElseNode& node) 
{
    std::shared_ptr<SemanticScope> else_body_scope = std::make_shared<SemanticScope>( scope );
    NodeTypeChecker else_body_checker { else_body_scope };
    else_body_checker.fn_return_type = this->fn_return_type;

    for ( const auto& bd_node : node.body_nodes )
    {    
        bd_node->accept( else_body_checker );   
    }

    this->isReturnSafe = else_body_checker.isReturnSafe;
}

void NodeTypeChecker::visit(const ReturnNode& node) 
{
    this->isReturnSafe = true;

    ExpressionTypeEvaluator type_evaluator { scope };

    if( this->fn_return_type != type_evaluator.evaluateType( *(node.ret_expr) ) )
    {
        throw std::runtime_error("Type error: Return statement expression type does not match defined function return type.");
    }
}

void NodeTypeChecker::visit(const ObjectSignatureNode& node) 
{
    const auto num_members = node.member_variables.size();
    
    std::vector<std::string> mem_types;
    mem_types.reserve(num_members);

    std::vector<std::string> mem_names;
    mem_names.reserve(num_members);

    std::vector<bool> is_obj_vec {};
    is_obj_vec.reserve(num_members);
   
    for ( const auto& mem : node.member_variables )
    {
        const auto* primitive_decl = dynamic_cast<const DeclarationNode*>( mem.get() );
        if ( primitive_decl )
        {
            mem_types.push_back(primitive_decl->type);
            mem_names.push_back(std::string(primitive_decl->name));
            is_obj_vec.push_back(false);
            continue;
        }

        const auto* obj_decl = dynamic_cast<const ObjectDeclarationNode*>( mem.get() );
        if ( obj_decl )
        {
            mem_types.push_back(std::string(obj_decl->type));
            mem_names.push_back(std::string(obj_decl->name));
            is_obj_vec.push_back(true);
        }
    }
    ObjectSignatureData data { num_members, std::move( mem_types ), std::move( mem_names ), std::move(is_obj_vec) };

    scope->defineObjectSignature( node.name, std::move(data) );
    auto x = scope->getObjectSignatureData(node.name);
}

void NodeTypeChecker::visit(const ObjectDeclarationNode& node)
{
    scope->defineObjectInstanceType(node.name, node.type);
}

void NodeTypeChecker::checkTypeUpCasting(std::string from, std::string to)
{
    if ( isNumeric(from) && isNumeric(to) )
    {
        int from_rank = getTypeConversionRank(from);
        int to_rank = getTypeConversionRank(to);
        
        if ( to_rank < from_rank )
        {
            throw std::runtime_error("Type error: Implicit type demotion from '" + from + "' to '" + to + "' is not allowed.");
        }
    }
    else
    {
        throw std::runtime_error("Type error: Declared type '" + to + "' is incompatible with '" + from + "'.");
    }
}

bool NodeTypeChecker::isNumeric(std::string tp)
{
    if (tp == "int" || 
        tp == "float" || 
        tp == "double" || 
        tp == "bool" )
    {
        return true;
    }
    return false;
}

int NodeTypeChecker::getTypeConversionRank(std::string tp)
{
    if ( tp == "bool" )     return 0;
    if ( tp == "int" )      return 1;
    if ( tp == "float" )    return 2;
    if ( tp == "double" )   return 3;
    return -1;
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
    std::string var_type = scope->lookup( expr.name ).type;

    if ( var_type.empty() )
    {
        var_type = scope->getObjectInstanceType( expr.name );
        
        if ( var_type.empty() )
        {
            throw std::runtime_error("Error: Variable '" + expr.name + "' is undefined.");
        }
    }

    last_evaluated_type = var_type;
}

void ExpressionTypeEvaluator::visit(const BinaryExpression& expr)
{

    std::string left_type = this->evaluateType( *(expr.expr_left) );
    std::string right_type = this->evaluateType( *(expr.expr_right) );

    // If both expresssions are numeric we will allow implicit casting
    if ( NodeTypeChecker::isNumeric(left_type) && NodeTypeChecker::isNumeric(right_type) )
    {   
        if ( expr.expr_operator == "and" || expr.expr_operator == "or" )
        {
            throw std::runtime_error("Type error: Operator '" + expr.expr_operator + "' is not compatible with numeric expressions.");
        }

        int highest_rank = std::max( NodeTypeChecker::getTypeConversionRank(left_type),
                                     NodeTypeChecker::getTypeConversionRank(right_type) );
        std::string highest_rank_type = NodeTypeChecker::getInverseTypeConversionRank(highest_rank);

        last_evaluated_type = highest_rank_type;
    }
    else if ( left_type == "string" && right_type == "string" )
    {
        if ( expr.expr_operator != "+" )
        {
            // Only allowed operator for two strings is '+'
            throw std::runtime_error("Type error: Operator '" + expr.expr_operator + "' is not compatible with string expressions.");
        }
        
        last_evaluated_type = left_type;
    }
    else
    {
        // String and numeric case
        throw std::runtime_error("Type error: Implicit type conversion not allowed. Evaluated types: '" + left_type + "', '" + right_type + "'.");
    }
    
}

void ExpressionTypeEvaluator::visit(const UnaryExpression& expr)
{
    last_evaluated_type = this->evaluateType( *(expr.child) );
}

void ExpressionTypeEvaluator::visit(const FunctionCallExpression& expr)
{
    std::string fn_ret_type = scope->lookup( expr.name ).function_return_type;
    
    if ( fn_ret_type.empty() )
    {
        throw std::runtime_error("Error: Function '" + expr.name + "' is undefined.");
    }

    auto fn_metadata = scope->getFnArgTypes( expr.name );
    
    auto num_args = fn_metadata.size();
    auto num_args_provided = expr.args.size(); 

    if ( num_args != num_args_provided )
    {
        throw std::runtime_error("Error: Invalid number of arguments for function call provided. Provided: '" + std::to_string(num_args_provided) + "', expected: '" + std::to_string(num_args) + "'.");
    }

    for ( std::size_t i {}; i < num_args; i++ )
    {
        std::string arg_type = this->evaluateType( *(expr.args[i]) );

        if ( fn_metadata[i] != arg_type )
        {
            NodeTypeChecker::checkTypeUpCasting( arg_type, fn_metadata[i] );
            //throw std::runtime_error("Type error: The type of argument " + std::to_string(i + 1) + " '" + arg_type + "' is incompatible with '" + fn_metadata[i] + "'.");
        }
    }

    last_evaluated_type = fn_ret_type;
}

void ExpressionTypeEvaluator::visit(const AccessMemberExpression& expr) 
{
    const auto* obj = dynamic_cast<VariableExpression*>(expr.left_side_expr.get());

    std::string obj_tp;

    if ( !obj )
    {
        // This is the case when we access nested object members eg. foo.faz.x
        if ( const auto* obj_member = dynamic_cast<AccessMemberExpression*>(expr.left_side_expr.get()) )
        {
            obj_tp = evaluateType(*obj_member);
        }
        else
        {
            // Function calls will be checked here
            throw std::runtime_error("Error: Invalid use of member accessor.");
        }
    }
    else
    {   
        obj_tp = scope->getObjectInstanceType(obj->name);
    }

    const auto obj_signature_data = scope->getObjectSignatureData(obj_tp);

    for ( std::size_t i {}; i < obj_signature_data.num_members; ++i )
    {
        if ( obj_signature_data.member_names[i] == expr.name )
        {
            last_evaluated_type = obj_signature_data.member_types[i];
            return;
        }
    }

    throw std::runtime_error("Error: Object does not name member '" + expr.name + "'.");
}

void ExpressionTypeEvaluator::visit(const MultiExpression& )
{

}