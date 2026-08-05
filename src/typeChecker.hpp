#ifndef TYPE_CHECKER_HPP
#define TYPE_CHECKER_HPP

#include<vector>
#include<memory>
#include<unordered_map>


class NodeTypeChecker;
class ASTNode;
class SemanticScope;

class TypeChecker
{
private: 
    SemanticScope& scope;
public:
    void run_check(const std::vector<std::unique_ptr<ASTNode>>& nodes);
    explicit TypeChecker(SemanticScope& s) : scope(s) {}
};

#endif
