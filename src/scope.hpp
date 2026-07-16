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

    void define(std::string_view name, std::shared_ptr<Declaration> decl) 
    {
        symbols[std::string(name)] = std::move(decl);
    }

    // TODO: Lookup should be forwarded to parent if unsuccesful. 
    std::shared_ptr<Declaration> lookup(std::string_view name) const;

    explicit Scope(); 
    explicit Scope(Scope& parent);

};

#endif