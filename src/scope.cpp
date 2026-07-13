#include"scope.hpp"

Scope::Scope() = default;
Scope::Scope(Scope& parent)
{
    this->parent_scope = &parent;
}

std::shared_ptr<Declaration> Scope::lookup(std::string_view name) const 
{
    auto it = symbols.find(std::string(name));
    if (it == symbols.end()) 
    {
        return nullptr;
    }
    return it->second;
}