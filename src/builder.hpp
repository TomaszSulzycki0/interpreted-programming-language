#ifndef BUILDER_HPP
#define BUILDER_HPP

#include<memory>
#include<vector>

class Scope;
class Declaration;
class ASTNode;

class Builder 
{
private:
    Scope& scope;
public:
    explicit Builder(Scope& s) : scope(s) {}
    void buildProgram(const std::vector<std::unique_ptr<ASTNode>>& ast); 
};

#endif