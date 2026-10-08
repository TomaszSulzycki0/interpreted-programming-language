#include"globalDeclarer.hpp"
#include"builder.hpp"
#include"typeChecker.hpp"
#include"abstractNodes.hpp"
#include"expression.hpp"

void GlobalDeclarer::defineBuiltInFunctions(std::shared_ptr<Scope>& scope, std::shared_ptr<SemanticScope>& s_scope)
{
    NodeMaker declarer { scope };
    NodeTypeChecker semantic_declarer { s_scope };

    std::vector<std::unique_ptr<DeclarationNode>> args;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    // print

    args.emplace_back(std::make_unique<DeclarationNode>(ImplementedType::_string, "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedPrintFunctionNode>());
    
    static std::unique_ptr<FunctionDeclarationNode> printNode = std::make_unique<FunctionDeclarationNode>
    ( 
        ImplementedType::_void, "print", std::move(args), std::move(body)
    );
    
    declarer.visit( *printNode );
    semantic_declarer.visit( *printNode );

    // bool cast

    args.emplace_back(std::make_unique<DeclarationNode>(ImplementedType::_double, "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>(ImplementedType::_bool));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>( std::make_unique<VariableAccess>("ret") ) ));
    
    static std::unique_ptr<FunctionDeclarationNode> boolCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        ImplementedType::_bool, "bool", std::move(args), std::move(body)
    );

    declarer.visit( *boolCastNode );
    semantic_declarer.visit( *boolCastNode );

    // int cast

    args.emplace_back(std::make_unique<DeclarationNode>(ImplementedType::_double, "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>(ImplementedType::_int));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>( std::make_unique<VariableAccess>("ret") ) ));
    
    static std::unique_ptr<FunctionDeclarationNode> intCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        ImplementedType::_int, "int", std::move(args), std::move(body)
    );
    
    declarer.visit( *intCastNode );
    semantic_declarer.visit( *intCastNode );

    // float cast

    args.emplace_back(std::make_unique<DeclarationNode>(ImplementedType::_double, "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>(ImplementedType::_float));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>( std::make_unique<VariableAccess>("ret") ) ));
    
    static std::unique_ptr<FunctionDeclarationNode> floatCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        ImplementedType::_float, "float", std::move(args), std::move(body)
    );

    declarer.visit( *floatCastNode );
    semantic_declarer.visit( *floatCastNode );

    // double cast

    args.emplace_back(std::make_unique<DeclarationNode>(ImplementedType::_double, "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>(ImplementedType::_double));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>( std::make_unique<VariableAccess>("ret") ) ));
    
    static std::unique_ptr<FunctionDeclarationNode> doubleCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        ImplementedType::_double, "double", std::move(args), std::move(body)
    );

    declarer.visit( *doubleCastNode );
    semantic_declarer.visit( *doubleCastNode );
    
    // string conversion from numerics

    args.emplace_back(std::make_unique<DeclarationNode>(ImplementedType::_double, "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>(ImplementedType::_string));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>( std::make_unique<VariableAccess>("ret") ) ));
    
    static std::unique_ptr<FunctionDeclarationNode> stringCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        ImplementedType::_string, "string", std::move(args), std::move(body)
    );

    declarer.visit( *stringCastNode );
    semantic_declarer.visit( *stringCastNode );

}