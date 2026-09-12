#include <gtest/gtest.h>

#include "parser.hpp"
#include "abstractNodes.hpp"
#include "expression.hpp"


TEST(ParserDeclarationTest, ParsesUninitializedDeclaration)
{
    const std::string code = R"(int x;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);
    EXPECT_TRUE(parser.isASTExecutable());

    const auto* node = dynamic_cast<const DeclarationNode*>(ast[0].get());
    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->type, "int");
    EXPECT_EQ(node->name, "x");
    EXPECT_EQ(node->initializer, nullptr);
}

TEST(ParserDeclarationTest, ParsesInitializedDeclaration)
{
    const std::string code = R"(int x = 42;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);
    EXPECT_TRUE(parser.isASTExecutable());

    const auto* node = dynamic_cast<const DeclarationNode*>(ast[0].get());
    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->type, "int");
    EXPECT_EQ(node->name, "x");

    const auto* literal = dynamic_cast<const LiteralExpression*>(node->initializer.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, "42");
    EXPECT_EQ(literal->tp, "int");
}

TEST(ParserDeclarationTest, ParsesFloatDeclaration)
{
    const std::string code = R"(float x = 3.14;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const DeclarationNode*>(ast[0].get());
    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->type, "float");
    EXPECT_EQ(node->name, "x");

    const auto* literal = dynamic_cast<const LiteralExpression*>(node->initializer.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, "3.14");
    EXPECT_EQ(literal->tp, "float");
}

TEST(ParserDeclarationTest, ParsesBooleanDeclaration)
{
    const std::string code = R"(bool enabled = true;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const DeclarationNode*>(ast[0].get());
    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->type, "bool");
    EXPECT_EQ(node->name, "enabled");

    const auto* literal = dynamic_cast<const LiteralExpression*>(node->initializer.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, "true");
    EXPECT_EQ(literal->tp, "bool");
}

TEST(ParserDeclarationTest, ParsesStringDeclaration)
{
    const std::string code = R"(string message = "hello";)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const DeclarationNode*>(ast[0].get());
    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->type, "string");
    EXPECT_EQ(node->name, "message");

    const auto* literal = dynamic_cast<const LiteralExpression*>(node->initializer.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, "\"hello\"");
    EXPECT_EQ(literal->tp, "string");
}

TEST(ParserDeclarationTest, ParsesEmptyStringDeclaration)
{
    const std::string code = R"(string message = "";)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const DeclarationNode*>(ast[0].get());
    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->initializer, nullptr);

    const auto* literal = dynamic_cast<const LiteralExpression*>(node->initializer.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, "");
    EXPECT_EQ(literal->tp, "string");
}

TEST(ParserDeclarationTest, ParsesMultipleDeclarations)
{
    const std::string code = R"(
        int x = 1;
        float y = 2.5;
        bool enabled = true;
        string name = "test";
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 4);
    EXPECT_TRUE(parser.isASTExecutable());

    const auto* first = dynamic_cast<const DeclarationNode*>(ast[0].get());
    const auto* second = dynamic_cast<const DeclarationNode*>(ast[1].get());
    const auto* third = dynamic_cast<const DeclarationNode*>(ast[2].get());
    const auto* fourth = dynamic_cast<const DeclarationNode*>(ast[3].get());

    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    ASSERT_NE(third, nullptr);
    ASSERT_NE(fourth, nullptr);

    EXPECT_EQ(first->type, "int");
    EXPECT_EQ(first->name, "x");

    EXPECT_EQ(second->type, "float");
    EXPECT_EQ(second->name, "y");

    EXPECT_EQ(third->type, "bool");
    EXPECT_EQ(third->name, "enabled");

    EXPECT_EQ(fourth->type, "string");
    EXPECT_EQ(fourth->name, "name");
}