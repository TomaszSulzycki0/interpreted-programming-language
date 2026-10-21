#ifndef SCOPE_HPP
#define SCOPE_HPP

#include<vector>
#include<memory>
#include<unordered_map>

class Declaration;

// Stores data for user defined types
struct ObjectSignatureData
{
    std::size_t num_members {};
    std::vector<std::string> member_types {};
    std::vector<std::string> member_names {};
    std::vector<bool> is_obj {};
};

// Scope class for declarations
class Scope
{
private:
    std::shared_ptr<Scope> parent_scope = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Declaration>> symbols;
    std::unordered_map<std::string, ObjectSignatureData> obj_data;

public:

    void printScope() const;

    void define(std::string_view name, std::shared_ptr<Declaration> decl) 
    {
        symbols[std::string(name)] = std::move(decl);
    }
    
    void defineObjectSignature(std::string_view type_name, ObjectSignatureData data) 
    {
        obj_data[std::string(type_name)] = data;
    }

    // Gets the types and names of object's members
    //  
    ObjectSignatureData getObjectSignatureData(std::string_view type_name) const;

    // Looks up the declaration in current or parent scopes
    std::shared_ptr<Declaration> lookup(std::string_view name) const;

    // Looks up the declaration in the current scope
    std::shared_ptr<Declaration> lookupLocal(std::string_view name) const;

    explicit Scope(); 
    explicit Scope(std::shared_ptr<Scope> parent);

};

// Stores the types / return types in case of functions
struct VariableData
{
    std::string type = "";
    std::string function_return_type = "";
};

// Scope class for running the type checker.
class SemanticScope
{
private:
    std::shared_ptr<SemanticScope> parent_scope = nullptr;
    std::unordered_map<std::string, VariableData> variable_types;
    std::unordered_map<std::string, std::vector<std::string>> fn_arg_data;
    std::unordered_map<std::string, ObjectSignatureData> obj_data;
    std::unordered_map<std::string, std::string> obj_instance_types;

public:

    void define(std::string_view name, VariableData data) 
    {
        variable_types[std::string(name)] = data;
    }

    void storeFnArgTypes(std::string_view name, std::vector<std::string> tps)
    {
        fn_arg_data[std::string(name)] = tps;
    }

    void defineObjectSignature(std::string_view type_name, ObjectSignatureData data) 
    {
        obj_data[std::string(type_name)] = data;
    }

    void defineObjectInstanceType(std::string_view name, std::string_view type) 
    {
        obj_instance_types[std::string(name)] = std::string(type);
    }

    ObjectSignatureData getObjectSignatureData(std::string_view type_name) const;

    std::string getObjectInstanceType(std::string_view name) const;

    std::vector<std::string> getFnArgTypes(std::string_view name) const;

    // Looks up the assosiated type in current or parent scopes
    VariableData lookup(std::string_view name) const;

    // Looks up the assosiated type in current scope
    VariableData lookupLocal(std::string_view name) const;

    explicit SemanticScope(); 
    explicit SemanticScope(std::shared_ptr<SemanticScope> parent);

};

#endif