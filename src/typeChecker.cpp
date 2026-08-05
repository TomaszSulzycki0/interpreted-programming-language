#include"typeChecker.hpp"
#include"visitors.hpp"
#include"abstractNodes.hpp"

void TypeChecker::run_check(const std::vector<std::unique_ptr<ASTNode>>& nodes)
{
    NodeTypeChecker node_visitor {scope};

    for ( const auto& node : nodes )
    {
        node->accept( node_visitor );
    }

}