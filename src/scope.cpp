#include"scope.hpp"
#include"visitors.hpp"
#include"declarationBase.hpp"

Scope::Scope() = default;
Scope::Scope(std::shared_ptr<Scope> parent) : parent_scope(std::move(parent)) {}

std::shared_ptr<Declaration> Scope::lookup(std::string_view name) const 
{
    auto it = symbols.find( std::string(name) );
    if ( it == symbols.end() ) 
    {
        if ( parent_scope != nullptr )
        {
            return parent_scope->lookup(name);
        }
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<Declaration> Scope::lookupLocal(std::string_view name) const 
{
    auto it = symbols.find( std::string(name) );
    if (it == symbols.end()) 
    {
        return nullptr;
    }
    return it->second;
}

SemanticScope::SemanticScope() = default;
SemanticScope::SemanticScope(std::shared_ptr<SemanticScope> parent) : parent_scope(std::move(parent)) {}

std::vector<std::string> SemanticScope::getFnArgTypes(std::string_view name) const
{
    auto it = fn_arg_data.find( std::string(name) );
    if ( it == fn_arg_data.end() )
    {
        // Should not be possible in practice
        throw std::runtime_error("Unexpected error during function argument type lookup");
    }

    return it->second;
}

std::string SemanticScope::lookup(std::string_view name) const 
{
    auto it = variable_types.find( std::string(name) );
    if ( it == variable_types.end() ) 
    {
        if ( parent_scope != nullptr )
        {
            return parent_scope->lookup(name);
        }
        return "";
    }
    return it->second;
}

std::string SemanticScope::lookupLocal(std::string_view name) const 
{
    auto it = variable_types.find( std::string(name) );
    if ( it == variable_types.end() ) 
    {
        return "";
    }
    return it->second;
}