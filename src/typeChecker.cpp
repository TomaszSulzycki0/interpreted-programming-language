#include"typeChecker.hpp"
#include"visitors.hpp"
#include"abstractNodes.hpp"

void TypeChecker::run_check(const std::vector<std::unique_ptr<ASTNode>>& nodes)
{
    NodeTypeChecker node_visitor {scope};

    for ( const auto& node : nodes )
    {
        try
        {
            node->accept( node_visitor );
        }
        catch ( const std::exception& e )
        {
            std::cerr << e.what() << '\n';
            is_well_typed = false;
        }
    }
}