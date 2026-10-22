#include"core/visitorsBase.hpp"
#include"code_executor/builder.hpp"
#include"checker/typeChecker.hpp"
#include"core/expression.hpp"
#include"core/scope.hpp"
#include"core/numericVariable.hpp"
#include"core/stringVariable.hpp"
#include"core/abstractNodes.hpp"
#include"debugMacros.hpp"

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

void PrintVisitor::visit(const ObjectDeclaration& ) 
{
    
}
