#include <gtest/gtest.h>

#include<vector>
#include<memory>

#include"core/abstractNodes.hpp"
#include"parser/parser.hpp"
#include"checker/typeChecker.hpp"
#include"core/scope.hpp"
#include"code_executor/builder.hpp"

class ObjectSignatureTest : public ::testing::Test
{
protected:
    std::vector<std::unique_ptr<ASTNode>> ast;

    void generateAST(const std::string& code)
    {
        Parser parser { code };
        parser.tokenizeProgram();
        ast = parser.parseProgram( PARSING_MODE::DEFAULT );

        ASSERT_TRUE( parser.isASTExecutable() );

        TypeChecker typechecker { std::make_unique<SemanticScope>() };
        typechecker.run_check(ast);
    
        ASSERT_TRUE( typechecker.isASTWellTyped() );
    }

    void TearDown() override 
    {
        // Set the keywords map of the tokenizer to default stat
        Tokenizer::resetKeywords();
    }
    
};


TEST_F(ObjectSignatureTest, DeclaresMembersWithCorrectType)
{
    const std::string code = R"(
        object foo {
            bool b;
            int i;
            float f;
            double d;
            string s;
        }
    )";

    generateAST(code);

    std::shared_ptr<Scope> main_scope { std::make_shared<Scope>() };

    Builder builder { main_scope };
    builder.buildProgram( ast );

    const auto obj_s = main_scope->getObjectSignatureData("foo");

    ASSERT_EQ(obj_s.num_members, 5);

    ASSERT_EQ(obj_s.num_members, obj_s.member_names.size());
    ASSERT_EQ(obj_s.num_members, obj_s.member_types.size());

    EXPECT_EQ(obj_s.member_names[0], "b");
    EXPECT_EQ(obj_s.member_names[1], "i");
    EXPECT_EQ(obj_s.member_names[2], "f");
    EXPECT_EQ(obj_s.member_names[3], "d");
    EXPECT_EQ(obj_s.member_names[4], "s");
    
    EXPECT_EQ(obj_s.member_types[0], "bool");
    EXPECT_EQ(obj_s.member_types[1], "int");
    EXPECT_EQ(obj_s.member_types[2], "float");
    EXPECT_EQ(obj_s.member_types[3], "double");
    EXPECT_EQ(obj_s.member_types[4], "string");

}

TEST_F(ObjectSignatureTest, DeclaresMemberObjects)
{
    const std::string code = R"(
        object foo {
            int i;
        }

        object faz {
            foo myFoo;
        }
    )";

    generateAST(code);

    std::shared_ptr<Scope> main_scope { std::make_shared<Scope>() };

    Builder builder { main_scope };
    builder.buildProgram( ast );

    const auto obj_s = main_scope->getObjectSignatureData("faz");

    ASSERT_EQ(obj_s.num_members, 1);

    ASSERT_EQ(obj_s.num_members, obj_s.member_names.size());
    ASSERT_EQ(obj_s.num_members, obj_s.member_types.size());

    EXPECT_EQ(obj_s.member_names[0], "myFoo");
    EXPECT_EQ(obj_s.member_types[0], "foo");

}