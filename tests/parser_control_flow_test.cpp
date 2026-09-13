#include <gtest/gtest.h>

#include "parser.hpp"
#include "abstractNodes.hpp"
#include "expression.hpp"


TEST(ParserControlFlowTest, ParsesIfWithoutElse)
{
    const std::string code = R"(
        if (x > 0) {
            x = 1;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);
    EXPECT_TRUE(parser.isASTExecutable());

    const auto* node = dynamic_cast<const IfNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->condition_expr, nullptr);
    EXPECT_EQ(node->else_nd, nullptr);

    const auto* condition = dynamic_cast<const BinaryExpression*>(node->condition_expr.get());

    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->expr_operator, ">");

    ASSERT_EQ(node->body_nodes.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(node->body_nodes[0].get());

    ASSERT_NE(assignment, nullptr);
    EXPECT_EQ(assignment->name, "x");
}

TEST(ParserControlFlowTest, ParsesIfWithElse)
{
    const std::string code = R"(
        if (x > 0) {
            x = 1;
        } else {
            x = 2;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const IfNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->else_nd, nullptr);

    ASSERT_EQ(node->body_nodes.size(), 1);
    ASSERT_EQ(node->else_nd->body_nodes.size(), 1);

    const auto* if_assignment = dynamic_cast<const AssignmentNode*>(node->body_nodes[0].get());
    const auto* else_assignment = dynamic_cast<const AssignmentNode*>(node->else_nd->body_nodes[0].get());

    ASSERT_NE(if_assignment, nullptr);
    ASSERT_NE(else_assignment, nullptr);

    EXPECT_EQ(if_assignment->name, "x");
    EXPECT_EQ(else_assignment->name, "x");

    const auto* if_value = dynamic_cast<const LiteralExpression*>(if_assignment->value_expr.get());
    const auto* else_value = dynamic_cast<const LiteralExpression*>(else_assignment->value_expr.get());

    ASSERT_NE(if_value, nullptr);
    ASSERT_NE(else_value, nullptr);

    EXPECT_EQ(if_value->value, "1");
    EXPECT_EQ(else_value->value, "2");
}

TEST(ParserControlFlowTest, ParsesWhile)
{
    const std::string code = R"(
        while (x < 10) {
            x += 1;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);
    EXPECT_TRUE(parser.isASTExecutable());

    const auto* node = dynamic_cast<const WhileNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->condition_expr, nullptr);

    const auto* condition = dynamic_cast<const BinaryExpression*>(node->condition_expr.get());

    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->expr_operator, "<");

    ASSERT_EQ(node->body_nodes.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(node->body_nodes[0].get());

    ASSERT_NE(assignment, nullptr);
    EXPECT_EQ(assignment->name, "x");
}

TEST(ParserControlFlowTest, ParsesNestedIf)
{
    const std::string code = R"(
        if (x > 0) {
            if (y > 0) {
                x = 1;
            }
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* outer = dynamic_cast<const IfNode*>(ast[0].get());

    ASSERT_NE(outer, nullptr);
    ASSERT_EQ(outer->body_nodes.size(), 1);

    const auto* inner = dynamic_cast<const IfNode*>(outer->body_nodes[0].get());

    ASSERT_NE(inner, nullptr);
    ASSERT_EQ(inner->body_nodes.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(inner->body_nodes[0].get());

    ASSERT_NE(assignment, nullptr);
    EXPECT_EQ(assignment->name, "x");
}

TEST(ParserControlFlowTest, ParsesFunctionDeclaration)
{
    const std::string code = R"(
        fn add(int a, int b) -> int {
            return a + b;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const FunctionDeclarationNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->name, "add");
    EXPECT_EQ(node->return_type, "int");

    ASSERT_EQ(node->arg_nodes.size(), 2);

    ASSERT_NE(node->arg_nodes[0], nullptr);
    ASSERT_NE(node->arg_nodes[1], nullptr);

    EXPECT_EQ(node->arg_nodes[0]->type, "int");
    EXPECT_EQ(node->arg_nodes[0]->name, "a");

    EXPECT_EQ(node->arg_nodes[1]->type, "int");
    EXPECT_EQ(node->arg_nodes[1]->name, "b");

    ASSERT_EQ(node->body_nodes.size(), 1);

    const auto* return_node = dynamic_cast<const ReturnNode*>(node->body_nodes[0].get());

    ASSERT_NE(return_node, nullptr);
}

TEST(ParserControlFlowTest, ParsesFunctionDeclarationWithoutArguments)
{
    const std::string code = R"(
        fn getValue() -> int {
            return 42;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const FunctionDeclarationNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->name, "getValue");
    EXPECT_EQ(node->return_type, "int");
    EXPECT_TRUE(node->arg_nodes.empty());

    ASSERT_EQ(node->body_nodes.size(), 1);

    const auto* return_node = dynamic_cast<const ReturnNode*>(node->body_nodes[0].get());

    ASSERT_NE(return_node, nullptr);
}

TEST(ParserControlFlowTest, ParsesFunctionWithMultipleBodyStatements)
{
    const std::string code = R"(
        fn test(int x) -> int {
            int y = 10;
            y = y + x;
            return y;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* function = dynamic_cast<const FunctionDeclarationNode*>(ast[0].get());

    ASSERT_NE(function, nullptr);

    ASSERT_EQ(function->arg_nodes.size(), 1);
    EXPECT_EQ(function->arg_nodes[0]->name, "x");

    ASSERT_EQ(function->body_nodes.size(), 3);

    EXPECT_NE(dynamic_cast<const DeclarationNode*>(function->body_nodes[0].get()), nullptr);
    EXPECT_NE(dynamic_cast<const AssignmentNode*>(function->body_nodes[1].get()), nullptr);
    EXPECT_NE(dynamic_cast<const ReturnNode*>(function->body_nodes[2].get()), nullptr);
}

TEST(ParserControlFlowTest, ParsesIfInsideWhile)
{
    const std::string code = R"(
        while (x < 10) {
            if (x > 5) {
                x = 0;
            }
            x += 1;
        }
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* while_node = dynamic_cast<const WhileNode*>(ast[0].get());

    ASSERT_NE(while_node, nullptr);
    ASSERT_EQ(while_node->body_nodes.size(), 2);

    EXPECT_NE(dynamic_cast<const IfNode*>(while_node->body_nodes[0].get()), nullptr);
    EXPECT_NE(dynamic_cast<const AssignmentNode*>(while_node->body_nodes[1].get()), nullptr);
}

TEST(ParserControlFlowTest, ParsesMultipleTopLevelStatements)
{
    const std::string code = R"(
        int x = 1;
        if (x > 0) {
            x = 2;
        }
        while (x < 10) {
            x += 1;
        }
        return x;
    )";

    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 4);

    EXPECT_NE(dynamic_cast<const DeclarationNode*>(ast[0].get()), nullptr);
    EXPECT_NE(dynamic_cast<const IfNode*>(ast[1].get()), nullptr);
    EXPECT_NE(dynamic_cast<const WhileNode*>(ast[2].get()), nullptr);
    EXPECT_NE(dynamic_cast<const ReturnNode*>(ast[3].get()), nullptr);
}

TEST(ParserControlFlowTest, ParsesEmptyReturn)
{
    const std::string code = R"(return;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);
    EXPECT_TRUE(parser.isASTExecutable());

    const auto* node = dynamic_cast<const ReturnNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->ret_expr, nullptr);
}