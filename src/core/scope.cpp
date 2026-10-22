#include"core/scope.hpp"
#include"core/visitorsBase.hpp"
#include"core/declarationBase.hpp"

Scope::Scope() = default;
Scope::Scope(std::shared_ptr<Scope> parent) : parent_scope(std::move(parent)) {}

void Scope::printScope() const
{
    std::cout << "--- Scope debug ---\n";
    for ( const auto& decl : symbols )
    {
        std::cout << decl.first << "\n";
    }
    std::cout << "-------------------\n";
}

ObjectSignatureData Scope::getObjectSignatureData(std::string_view type_name) const
{
    auto it = obj_data.find( std::string(type_name) );
    if ( it == obj_data.end() ) 
    {
        if ( parent_scope != nullptr )
        {
            return parent_scope->getObjectSignatureData(type_name);
        }
        return ObjectSignatureData();
    }
    return it->second;

}

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
        if ( parent_scope != nullptr )
        {
            return parent_scope->getFnArgTypes(name);
        }

        throw std::runtime_error("Unexpected error during function argument type lookup");
    }

    return it->second;
}

VariableData SemanticScope::lookup(std::string_view name) const 
{
    auto it = variable_types.find( std::string(name) );
    if ( it == variable_types.end() ) 
    {
        if ( parent_scope != nullptr )
        {
            return parent_scope->lookup(name);
        }
        return VariableData();
    }
    return it->second;
}

VariableData SemanticScope::lookupLocal(std::string_view name) const 
{
    auto it = variable_types.find( std::string(name) );
    if ( it == variable_types.end() ) 
    {
        return VariableData();
    }
    return it->second;
}

ObjectSignatureData SemanticScope::getObjectSignatureData(std::string_view type_name) const
{
    auto it = obj_data.find( std::string(type_name) );
    if ( it == obj_data.end() ) 
    {
        if ( parent_scope != nullptr )
        {
            return parent_scope->getObjectSignatureData(type_name);
        }
        return ObjectSignatureData();
    }
    return it->second;

}

std::string SemanticScope::getObjectInstanceType(std::string_view name) const
{
    auto it = obj_instance_types.find( std::string(name) );
    if ( it == obj_instance_types.end() ) 
    {
        if ( parent_scope != nullptr )
        {
            return parent_scope->getObjectInstanceType(name);
        }
        return "";
    }
    return it->second;

}