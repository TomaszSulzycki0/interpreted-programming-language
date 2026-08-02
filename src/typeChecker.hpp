#ifndef TYPE_CHECKER_HPP
#define TYPE_CHECKER_HPP

#include<vector>
#include<memory>
#include<unordered_map>


class NodeTypeChecker;
class ASTNode;

// TODO: TypeChecker should check scope
class TypeChecker
{
public:
    void run_check(const std::vector<std::unique_ptr<ASTNode>>& nodes);
};

#endif
