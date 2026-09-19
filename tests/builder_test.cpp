#include <gtest/gtest.h>

#include"visitorsBase.hpp"
#include"abstractNodes.hpp"
#include"parser.hpp"
#include"typeChecker.hpp"
#include"scope.hpp"
#include"builder.hpp"
#include"declarationBase.hpp"


class BuilderTest : public ::testing::Test
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

    void checkDeclarationValueType(const std::shared_ptr<Declaration>& decl, const std::string& tp, const RuntimeValue& val)
    {
        ASSERT_NE(decl, nullptr);

        auto decl_val = std::dynamic_pointer_cast<ValueDeclaration>(decl);

        ASSERT_NE(decl_val, nullptr);

        EXPECT_EQ( decl_val->getType(), tp);

        EXPECT_EQ( decl_val->getValue(), val);
    }
    
};

class BuilderDeclarationTest : public BuilderTest
{    
};

class BuilderAssignmentTest : public BuilderTest
{    
};

TEST_F(BuilderDeclarationTest, DeclaresWithDefaultValue)
{
    const std::string code = R"(
        bool b;
        int x;
        float y;
        double d;
        string s;
    )";

    generateAST(code);

    std::shared_ptr<Scope> main_scope { std::make_shared<Scope>() };

    Builder builder { main_scope };
    builder.buildProgram( ast );

    checkDeclarationValueType( main_scope->lookup("b") , "bool", false);
    checkDeclarationValueType( main_scope->lookup("x") , "int", 0);
    checkDeclarationValueType( main_scope->lookup("y") , "float", (float)0);
    checkDeclarationValueType( main_scope->lookup("d") , "double", (double)0);
    checkDeclarationValueType( main_scope->lookup("s") , "string", "");
    
}

TEST_F(BuilderDeclarationTest, DeclaresWithExplicitValue)
{
    const std::string code = R"(
        bool b = true;
        int x = 10;
        float y = 9.12;
        double d = 21.9;
        string s = "abc";
    )";

    generateAST(code);

    std::shared_ptr<Scope> main_scope { std::make_shared<Scope>() };

    Builder builder { main_scope };
    builder.buildProgram( ast );

    checkDeclarationValueType( main_scope->lookup("b") , "bool", true);
    checkDeclarationValueType( main_scope->lookup("x") , "int", 10);
    checkDeclarationValueType( main_scope->lookup("y") , "float", (float)9.12);
    checkDeclarationValueType( main_scope->lookup("d") , "double", (double)21.9);
    checkDeclarationValueType( main_scope->lookup("s") , "string", "abc");
    
}

TEST_F(BuilderDeclarationTest, DeclaresFunctionCorrectly)
{
    const std::string code = R"(
        fn foo(bool a0, int a1, float a2, double a3, string a4) -> void {}
    )";

    generateAST(code);

    std::shared_ptr<Scope> main_scope { std::make_shared<Scope>() };

    Builder builder { main_scope };
    builder.buildProgram( ast );

    const auto foo = main_scope->lookup("foo");
    const auto foo_fn = std::dynamic_pointer_cast<FunctionDeclaration>(foo);

    ASSERT_NE(foo_fn, nullptr);

    ASSERT_EQ(foo_fn->arg_nodes.size(), 5);

}

TEST_F(BuilderAssignmentTest, AssignsExplicitValue)
{
    const std::string code = R"(
        bool b;
        int x;
        float y;
        double d;
        string s;

        b = true;
        x = 100;
        y = 10.1;
        d = 0.01;
        s = "abc";
    )";

    generateAST(code);

    std::shared_ptr<Scope> main_scope { std::make_shared<Scope>() };

    Builder builder { main_scope };
    builder.buildProgram( ast );

    checkDeclarationValueType( main_scope->lookup("b") , "bool", true);
    checkDeclarationValueType( main_scope->lookup("x") , "int", 100);
    checkDeclarationValueType( main_scope->lookup("y") , "float", (float)10.1);
    checkDeclarationValueType( main_scope->lookup("d") , "double", (double)0.01);
    checkDeclarationValueType( main_scope->lookup("s") , "string", "abc");

}