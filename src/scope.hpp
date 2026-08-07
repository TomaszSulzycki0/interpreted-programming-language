#ifndef SCOPE_HPP
#define SCOPE_HPP

#include<vector>
#include<memory>
#include<unordered_map>

class Declaration;

// Scope class for declarations
class Scope
{
private:
    std::shared_ptr<Scope> parent_scope = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Declaration>> symbols;

public:

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
class SemanticScope
{
private:
    std::shared_ptr<SemanticScope> parent_scope = nullptr;
    std::unordered_map<std::string, std::string> variable_types;

public:

    void define(std::string_view name, std::string_view tp) 
    {
        variable_types[std::string(name)] = std::string(tp);
    }

    // Looks up the assosiated type in current or parent scopes
    std::string lookup(std::string_view name) const;

    // Looks up the assosiated type in current scope
    std::string lookupLocal(std::string_view name) const;

    explicit SemanticScope(); 
    explicit SemanticScope(std::shared_ptr<SemanticScope> parent);

};

#endif