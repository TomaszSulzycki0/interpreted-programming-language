#ifndef SCOPE_HPP
#define SCOPE_HPP

#include<vector>
#include<memory>
#include<unordered_map>

class Declaration;

class Scope
{
private:
    Scope* parent_scope = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Declaration>> symbols;

public:

    void define(const std::string& name, std::shared_ptr<Declaration> decl) 
    {
        symbols[name] = std::move(decl);
    }

    // Lookup should be forwarded to parent if unsuccesful. 
    std::shared_ptr<Declaration> lookup(const std::string& name) const 
    {
        auto it = symbols.find(name);
        if (it == symbols.end()) 
        {
            throw std::runtime_error("Error: Variable '" + name + "' is undefined.");
        }
        return it->second;
    }

    Scope(); 
    Scope(Scope& parent);

};

#endif