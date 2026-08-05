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

    // If checker throws at any point, 
    // it continues checking nodes but marks itself as non executable
    bool is_well_typed = true;
    
public:
    void run_check(const std::vector<std::unique_ptr<ASTNode>>& nodes);
    bool isASTWellTyped() const { return is_well_typed; }

    explicit TypeChecker(SemanticScope& s) : scope(s) {}
};

#endif
