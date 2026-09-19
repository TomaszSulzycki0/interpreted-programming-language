#include<format>
#include<charconv>

#include"visitorsBase.hpp"
#include"builder.hpp"
#include"typeChecker.hpp"
#include"expression.hpp"
#include"scope.hpp"
#include"numericVariable.hpp"
#include"stringVariable.hpp"
#include"abstractNodes.hpp"
#include"debugMacros.hpp"
#include"implementedType.hpp"

std::ostream& operator<<(std::ostream& os, const RuntimeValue& value) 
{
    std::visit([&os](const auto& arg) 
    {
        using EvaluatedType = std::decay_t<decltype(arg)>;
        if constexpr ( std::is_same_v<EvaluatedType, bool> ) 
        {
            os << (arg ? "true" : "false");
        } 
        else 
        {
            os << arg;
        }

    }, value);
    
    return os;
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