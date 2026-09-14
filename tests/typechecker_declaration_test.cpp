#include <gtest/gtest.h>

#include"typeChecker.hpp"
#include"visitors.hpp"
#include"abstractNodes.hpp"
#include"parser.hpp"
#include"scope.hpp"

class TypecheckerDeclaratonTest : public ::testing::Test
{
protected:
    std::shared_ptr<SemanticScope> main_semantic_scope = std::make_shared<SemanticScope>();

    TypeChecker typechecker { main_semantic_scope };

    bool typechecks(const std::string& code)
    {
        Parser parser { code };
        parser.tokenizeProgram();

        const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

        EXPECT_TRUE(parser.isASTExecutable());

        typechecker.run_check(ast);

        return typechecker.isASTWellTyped();
    }
};

TEST_F(TypecheckerDeclaratonTest, AllowsInitializationWithGoodFunctionCall)
{
    EXPECT_TRUE(typechecks(R"(
        fn rBool() -> bool { return true; } 
        fn rInt() -> int { return 1; } 
        fn rFloat() -> float { return 1.0; } 
        fn rDouble() -> double { return 1.0; } 
        fn rString() -> string { return "true"; } 
        bool b = rBool(); 
        int x = rInt(); 
        float y = rFloat(); 
        double d = rDouble(); 
        string s = rString(); 
    )"));
}

TEST_F(TypecheckerDeclaratonTest, ForbidsBadInitializationWithImplicitDownCasting)
{
    EXPECT_FALSE(typechecks(R"(
        int i = 10.1;    
    )"));
}

TEST_F(TypecheckerDeclaratonTest, ForbidsBadInitializationWithUncastableValue)
{
    EXPECT_FALSE(typechecks(R"(
        int i = "foo";    
    )"));
}