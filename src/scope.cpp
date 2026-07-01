#include"scope.hpp"

Scope::Scope() = default;
Scope::Scope(Scope& parent)
{
    this->parent_scope = &parent;
}