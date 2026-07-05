#ifndef BUILDER_HPP
#define BUILDER_HPP

#include<memory>

class Scope;
class Declaration;
struct DeclarationNode;

class Builder {
private:
    Scope& scope;
public:
    explicit Builder(Scope& s) : scope(s) {}
    std::shared_ptr<Declaration> build(const DeclarationNode& node); 
};

#endif