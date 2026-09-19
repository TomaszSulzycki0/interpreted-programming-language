#include <gtest/gtest.h>

#include"typeChecker.hpp"
#include"visitorsBase.hpp"
#include"abstractNodes.hpp"
#include"parser.hpp"
#include"scope.hpp"

class TypecheckerScopeTest : public ::testing::Test
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

TEST_F(TypecheckerScopeTest, SeesVariableInParentScope)
{
    EXPECT_TRUE(typechecks(R"(
        int x;

        if ( 1 )
        {
            x += 1;
        }
        else
        {
            x += 1;
        }
        while ( 1 )
        {
            x += 1;
        }

        fn foo() -> void
        {
            x += 1;
        }
    )"));
}

TEST_F(TypecheckerScopeTest, SeesVariableInNestedParentScopes)
{
    EXPECT_TRUE(typechecks(R"(
        int x;
        if ( 1 )
        {
            if ( 1 )
            {
                if ( 1 )
                {
                    
                } 
                else
                {
                    x += 1;
                }       
            }   
        }
    )"));
}

TEST_F(TypecheckerScopeTest, AllowsVariableShadowing)
{
    EXPECT_TRUE(typechecks(R"(
        int x;

        fn foo() -> void
        {
            x += 1;
            int x = 1;
            x += 1;
        }
    )"));
}

TEST_F(TypecheckerScopeTest, ForbidsRedeclaration)
{
    EXPECT_FALSE(typechecks(R"(
        int x = 0;
        int x = 1;
    )"));
}

// Overloading not implemented
TEST_F(TypecheckerScopeTest, ForbidsFunctionRedeclaration)
{
    EXPECT_FALSE(typechecks(R"(
        fn foo() -> void {}
        fn foo() -> int { return 1;} 
    )"));
}

TEST_F(TypecheckerScopeTest, MissingReturnStatement)
{
    EXPECT_FALSE(typechecks(R"(
        fn foo() -> int {} 
    )"));
}

TEST_F(TypecheckerScopeTest, PotentialPathWithMissingReturnStatement)
{
    EXPECT_FALSE(typechecks(R"(
        fn foo() -> int 
        {
            if ( 1 )
            {
                return 0;
            }

            if ( 1 )
            {
                
            }
            else
            {
                return 1;
            }
        } 
    )"));
}

TEST_F(TypecheckerScopeTest, ReturnStatementReliesOnSatisfyingWhileLoopCondition)
{
    EXPECT_FALSE(typechecks(R"(
        fn foo() -> int 
        {
            bool not_true = false;

            while( not_true )
            {
                return 0;
            }
        } 
    )"));
}

TEST_F(TypecheckerScopeTest, ReturnStatementGuaranteedIfBothPathsLeadToReturn)
{
    EXPECT_TRUE(typechecks(R"(
        fn foo() -> int 
        {
            if ( 0 )
            {
                return 0;
            }
            else
            {
                return 1;
            }
        } 
    )"));
}

TEST_F(TypecheckerScopeTest, UndefinedVariableReference)
{
    EXPECT_FALSE(typechecks(R"(
        if ( 1 )
        {
            int y;
        }
        int x = y;
    )"));
}

TEST_F(TypecheckerScopeTest, UndefinedFunctionCall)
{
    EXPECT_FALSE(typechecks(R"(
        if ( 1 )
        {
            fn foo() -> int { return 1; }
        }
        int x = foo();
    )"));
}