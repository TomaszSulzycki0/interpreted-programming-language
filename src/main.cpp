#include<vector>
#include<iostream>
#include<string>
#include<filesystem>
#include<stdexcept>
#include<fstream>

#include"scope.hpp"
#include"numericVariable.hpp"
#include"parser.hpp"
#include"visitors.hpp"
#include"builder.hpp"
#include"typeChecker.hpp"

constexpr int num_args_for_default_usage = 2;
constexpr int num_args_for_description = 1;

void programDesc();
bool hasCorrectExtension(const std::string& filename, const std::string& expected_ext);
std::string readFileToString(const std::string& filename); 

void defineBuiltInFunctions(std::shared_ptr<Scope>& scope, std::shared_ptr<SemanticScope>& s_scope);

int main(int argc, char** argv)
{
    
    switch (argc)
    {
    case num_args_for_description:
        programDesc();
        return 0;
    case num_args_for_default_usage:
        break;
    default:
        std::cerr << "Error: Invalid number of parameters passed to interpreter: " << argc - 1 << std::endl;
        return -1;
    }

    std::string input_filename = argv[1];

    if( !hasCorrectExtension(input_filename, ".ipl") )
    {
        std::cerr << "Error: Invalid input file extension. Expecting .ipl" << std::endl;
        return -1;
    }

    try
    {
        Parser parser { readFileToString(input_filename) };

        std::shared_ptr<SemanticScope> main_semantic_scope { std::make_shared<SemanticScope>() };
        TypeChecker type_checker { main_semantic_scope };
        
        std::shared_ptr<Scope> main_scope { std::make_shared<Scope>() };
        Builder builder { main_scope };
        
        defineBuiltInFunctions( main_scope, main_semantic_scope );

        parser.tokenizeProgram();
        
        auto program_ast = parser.parseProgram( PARSING_MODE::DEFAULT );

        if ( parser.isASTExecutable() )
        {
            type_checker.run_check( program_ast );
        }
        
        if ( type_checker.isASTWellTyped() && parser.isASTExecutable() )
        {
            builder.buildProgram( program_ast ); 
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void programDesc()
{
    std::cout << "-----------------------\nInterpreted Programming Language\nPersonal Project\nUsage command: ilp filename.ilp\n-----------------------" << std::endl;
}

bool hasCorrectExtension(const std::string& filename, const std::string& expected_ext) 
{
    return std::filesystem::path(filename).extension() == expected_ext;
}

std::string readFileToString(const std::string& filename) 
{
    std::ifstream file_stream(filename);
    if (!file_stream.is_open()) 
    {
        throw std::runtime_error("Error: Unable to access file: \'" + filename + "\'.");
    }
    
    std::ostringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

void defineBuiltInFunctions(std::shared_ptr<Scope>& scope, std::shared_ptr<SemanticScope>& s_scope)
{
    NodeMaker declarer { scope };
    NodeTypeChecker semantic_declarer { s_scope };

    std::vector<std::unique_ptr<DeclarationNode>> args;
    std::vector<std::unique_ptr<ASTNode>> body;
    
    // print

    args.emplace_back(std::make_unique<DeclarationNode>("string", "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedPrintFunctionNode>());
    
    static std::unique_ptr<FunctionDeclarationNode> printNode = std::make_unique<FunctionDeclarationNode>
    ( 
        "void", "print", std::move(args), std::move(body)
    );
    
    declarer.visit( *printNode );
    semantic_declarer.visit( *printNode );

    // bool cast

    args.emplace_back(std::make_unique<DeclarationNode>("double", "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>("bool"));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>("ret") ));
    
    static std::unique_ptr<FunctionDeclarationNode> boolCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        "bool", "bool", std::move(args), std::move(body)
    );

    declarer.visit( *boolCastNode );
    semantic_declarer.visit( *boolCastNode );

    // int cast

    args.emplace_back(std::make_unique<DeclarationNode>("double", "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>("int"));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>("ret") ));
    
    static std::unique_ptr<FunctionDeclarationNode> intCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        "int", "int", std::move(args), std::move(body)
    );
    
    declarer.visit( *intCastNode );
    semantic_declarer.visit( *intCastNode );

    // float cast

    args.emplace_back(std::make_unique<DeclarationNode>("double", "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>("float"));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>("ret") ));
    
    static std::unique_ptr<FunctionDeclarationNode> floatCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        "float", "float", std::move(args), std::move(body)
    );

    declarer.visit( *floatCastNode );
    semantic_declarer.visit( *floatCastNode );

    // double cast

    args.emplace_back(std::make_unique<DeclarationNode>("double", "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>("double"));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>("ret") ));
    
    static std::unique_ptr<FunctionDeclarationNode> doubleCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        "double", "double", std::move(args), std::move(body)
    );

    declarer.visit( *doubleCastNode );
    semantic_declarer.visit( *doubleCastNode );
    
    // string conversion from numerics

    args.emplace_back(std::make_unique<DeclarationNode>("double", "data", nullptr));
    body.emplace_back(std::make_unique<EmbeddedCastFunctionNode>("string"));
    body.emplace_back(std::make_unique<ReturnNode>( std::make_unique<VariableExpression>("ret") ));
    
    static std::unique_ptr<FunctionDeclarationNode> stringCastNode = std::make_unique<FunctionDeclarationNode>
    ( 
        "string", "string", std::move(args), std::move(body)
    );

    declarer.visit( *stringCastNode );
    semantic_declarer.visit( *stringCastNode );

}