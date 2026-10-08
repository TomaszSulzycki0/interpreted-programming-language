#ifndef GLOBAL_DECL_HPP
#define GLOBAL_DECL_HPP

#include<memory>

class Scope;
class SemanticScope;

class GlobalDeclarer
{
public:
// Declares built-in functions in the global scope
static void defineBuiltInFunctions(std::shared_ptr<Scope>& scope, std::shared_ptr<SemanticScope>& s_scope);
    
};

#endif