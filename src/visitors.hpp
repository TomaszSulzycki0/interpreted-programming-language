#ifndef VISITORS_HPP
#define VISITORS_HPP

#include<iostream>

#include"variable.hpp"

class PrintVisitor : public DeclarationVisitor 
{
public:
    void visit(const NumericDeclaration& num_decl) override 
    {
        if (num_decl.isFloatingPoint()) 
        {
            std::cout << "(floating point) " << num_decl.asDouble();
        } else 
        {
            std::cout << "(integer) " << num_decl.asInteger();
        }
    }
};

#endif