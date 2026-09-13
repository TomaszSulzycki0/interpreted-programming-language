#include <gtest/gtest.h>

#include "parser.hpp"
#include "abstractNodes.hpp"
#include "expression.hpp"


TEST(ParserErrorTest, DoesNotParseBeforeTokenization)
{
    const std::string code = R"(int x = 42;)";
    Parser parser(code);

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_TRUE(ast.empty());
    EXPECT_TRUE(parser.isASTExecutable());
}

TEST(ParserErrorTest, DetectsInvalidAssignment)
{
    const std::string code = R"(x 42;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
    EXPECT_TRUE(ast.empty());
}

TEST(ParserErrorTest, DetectsMissingSemicolon)
{
    const std::string code = R"(int x = 42)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
    EXPECT_TRUE(ast.empty());
}

TEST(ParserErrorTest, DetectsInvalidDeclaration)
{
    const std::string code = R"(int 42;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
    EXPECT_TRUE(ast.empty());
}

TEST(ParserErrorTest, DetectsMissingIfParenthesis)
{
    const std::string code = R"(
        if x > 0 {
            x = 1;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
    EXPECT_TRUE(ast.empty());
}

TEST(ParserErrorTest, DetectsMissingIfOpeningBrace)
{
    const std::string code = R"(
        if (x > 0)
            x = 1;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
}

TEST(ParserErrorTest, DetectsMissingWhileOpeningBrace)
{
    const std::string code = R"(
        while (x < 10)
            x += 1;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
}

TEST(ParserErrorTest, DetectsMissingFunctionReturnType)
{
    const std::string code = R"(
        fn foo() {
            return 1;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
}

TEST(ParserErrorTest, DetectsMissingFunctionOpeningBrace)
{
    const std::string code = R"(
        fn foo() -> int
            return 1;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
}

TEST(ParserErrorTest, DetectsMissingFunctionClosingBrace)
{
    const std::string code = R"(
        fn foo() -> int {
            return 1;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
}

TEST(ParserErrorTest, DetectsMissingFunctionCallSemicolon)
{
    const std::string code = R"(foo())";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
}

TEST(ParserErrorTest, InvalidStatementDoesNotPreventLaterValidStatement)
{
    const std::string code = R"(
        @@@;
        int x = 42;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const DeclarationNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->name, "x");
}

TEST(ParserErrorTest, ParserRemainsExecutableAfterValidProgram)
{
    const std::string code = R"(
        int x = 1;
        x = x + 1;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 2);
    EXPECT_TRUE(parser.isASTExecutable());
}

TEST(ParserErrorTest, TokenizeProgramProvidesTokenCount)
{
    const std::string code = R"(int x = 42;)";
    Parser parser(code);

    EXPECT_EQ(parser.getNumTokens(), 0);

    parser.tokenizeProgram();

    EXPECT_GT(parser.getNumTokens(), 0);
}

TEST(ParserErrorTest, RejectsEmptyExpressions)
{
    const std::string code = R"(
        int x = ;
        x = ;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
}

TEST(ParserErrorTest, RejectsMalformedBinaryExpressions)
{
    const std::string code = R"(
        int x = 1 + ;
        x = * 2;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_FALSE(parser.isASTExecutable());
}

TEST(ParserErrorTest, HandlesUnaryAndBinaryExpressions)
{
    const std::string code = R"(
        int x = -2 * 3;
        int y = 1 + -2;
        int z = -x * y;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    EXPECT_TRUE(parser.isASTExecutable());
    EXPECT_EQ(ast.size(), 3);
}
