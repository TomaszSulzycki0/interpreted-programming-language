#include <gtest/gtest.h>

#include"typeChecker.hpp"
#include"visitors.hpp"
#include"abstractNodes.hpp"
#include"parser.hpp"
#include"scope.hpp"

class TypecheckerAssignmentTest : public ::testing::Test
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

TEST_F(TypecheckerAssignmentTest, AllowsAssignmentWithGoodFunctionCall)
{
    EXPECT_TRUE(typechecks(R"(
        fn rBool() -> bool { return true; } 
        fn rInt() -> int { return 1; } 
        fn rFloat() -> float { float x = 1.0; return x;} 
        fn rDouble() -> double { double y = 1.0; return y; } 
        fn rString() -> string { return "true"; } 

        bool b; 
        int x; 
        float y; 
        double d; 
        string s;

        b = rBool(); 
        x = rInt(); 
        y = rFloat(); 
        d = rDouble(); 
        s = rString(); 
    )"));
}

TEST_F(TypecheckerAssignmentTest, AllowsAssignmentWithImplicitTypeUpcasting)
{
    EXPECT_TRUE(typechecks(R"(

        bool b; 
        int x; 
        float y; 
        double d; 
        string s;

        x = b;

        y = b;
        y = x;

        d = b;
        d = x;
        d = y;
    )"));
}

TEST_F(TypecheckerAssignmentTest, ForbidsAssignmentWithImplicitTypeDowncasting)
{
    EXPECT_FALSE(typechecks(R"(
 
        float y; 
        double d; 

        y = d;
    )"));
}

TEST_F(TypecheckerAssignmentTest, ForbidsBadAssignmentWithUncastableValue)
{
    EXPECT_FALSE(typechecks(R"(
 
        int x; 
        string s = "aa"; 

        x = s;
    )"));
}
