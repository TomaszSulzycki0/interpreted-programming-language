#ifndef SCOPE_HPP
#define SCOPE_HPP

#include<vector>
#include<memory>
#include<unordered_map>

#include"implementedType.hpp"

class Declaration;

// Scope class for declarations
class Scope
{
private:
    std::shared_ptr<Scope> parent_scope = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Declaration>> symbols;

public:

    void printScope() const;

    void define(std::string_view name, std::shared_ptr<Declaration> decl) 
    {
        symbols[std::string(name)] = std::move(decl);
    }

    // Looks up the declaration in current or parent scopes
    std::shared_ptr<Declaration> lookup(std::string_view name) const;

    // Looks up the declaration in the current scope
    std::shared_ptr<Declaration> lookupLocal(std::string_view name) const;

    explicit Scope(); 
    explicit Scope(std::shared_ptr<Scope> parent);

};

// Scope class for running the type checker.
// Stores the types / return types in case of functions

struct VariableData
{
    ImplementedType type = ImplementedType::NULL_TYPE;
    ImplementedType function_return_type = ImplementedType::NULL_TYPE;
};

class SemanticScope
{
private:
    std::shared_ptr<SemanticScope> parent_scope = nullptr;
    std::unordered_map<std::string, VariableData> variable_types;
    std::unordered_map<std::string, std::vector<ImplementedType>> fn_arg_data;

public:

    void define(std::string_view name, VariableData data) 
    {
        variable_types[std::string(name)] = data;
    }

    void storeFnArgTypes(std::string_view name, std::vector<ImplementedType> tps)
    {
        fn_arg_data[std::string(name)] = tps;
    }

    std::vector<ImplementedType> getFnArgTypes(std::string_view name) const;

    // Looks up the assosiated type in current or parent scopes
    VariableData lookup(std::string_view name) const;

    // Looks up the assosiated type in current scope
    VariableData lookupLocal(std::string_view name) const;

    explicit SemanticScope(); 
    explicit SemanticScope(std::shared_ptr<SemanticScope> parent);

};

#endif