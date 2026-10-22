#include"interpreter.hpp"
#include"parser/parser.hpp"
#include"core/scope.hpp"
#include"checker/typeChecker.hpp"
#include"code_executor/builder.hpp"
#include"globalDeclarer.hpp"

#include<memory>

void Interpreter::run(const std::string& code)
{
    try
    {
        Parser parser { code };

        std::shared_ptr<SemanticScope> main_semantic_scope { std::make_shared<SemanticScope>() };
        TypeChecker type_checker { main_semantic_scope };
        
        std::shared_ptr<Scope> main_scope { std::make_shared<Scope>() };
        Builder builder { main_scope };
        
        GlobalDeclarer::defineBuiltInFunctions( main_scope, main_semantic_scope );

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