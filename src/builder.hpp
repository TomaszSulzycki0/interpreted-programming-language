#ifndef BUILDER_HPP
#define BUILDER_HPP

#include<memory>
#include<vector>

class Scope;
class Declaration;
struct DeclarationNode;

class Builder {
private:
    Scope& scope;
public:
    explicit Builder(Scope& s) : scope(s) {}
    std::shared_ptr<Declaration> buildNode(const DeclarationNode& node); 
    void buildProgram(const std::vector<std::unique_ptr<DeclarationNode>>& ast); 

};

#endif