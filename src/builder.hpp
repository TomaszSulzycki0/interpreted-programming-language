#ifndef BUILDER_HPP
#define BUILDER_HPP

#include<memory>
#include<vector>

class Scope;
class ASTNode;

class Builder 
{
private:
    std::shared_ptr<Scope> scope;
public:
    explicit Builder(std::shared_ptr<Scope> s) : scope(std::move(s)) {}
    void buildProgram(const std::vector<std::unique_ptr<ASTNode>>& ast); 
};

#endif