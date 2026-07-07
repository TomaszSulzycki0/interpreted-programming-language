#include"scope.hpp"

Scope::Scope() = default;
Scope::Scope(Scope& parent)
{
    this->parent_scope = &parent;
}

std::shared_ptr<Declaration> Scope::lookup(const std::string& name) const 
{
    auto it = symbols.find(name);
    if (it == symbols.end()) 
    {
        throw std::runtime_error("Error: Variable '" + name + "' is undefined.");
    }
    return it->second;
}