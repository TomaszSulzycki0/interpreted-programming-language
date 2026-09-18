#include <gtest/gtest.h>

#include "parser.hpp"
#include "abstractNodes.hpp"
#include "expression.hpp"
#include "implementedType.hpp"


TEST(ParserExpressionTest, ParsesSimpleAssignment)
{
    const std::string code = R"(x = 42;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);
    EXPECT_TRUE(parser.isASTExecutable());

    const auto* node = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->name, "x");

    const auto* literal = dynamic_cast<const LiteralExpression*>(node->value_expr.get());

    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, "42");
    EXPECT_EQ(literal->tp, ImplementedType::_int);
}

TEST(ParserExpressionTest, ParsesVariableAssignment)
{
    const std::string code = R"(x = y;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(node, nullptr);

    const auto* variable = dynamic_cast<const VariableExpression*>(node->value_expr.get());

    ASSERT_NE(variable, nullptr);
    EXPECT_EQ(variable->name, "y");
}

TEST(ParserExpressionTest, ParsesBinaryExpression)
{
    const std::string code = R"(x = 2 + 3;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(assignment, nullptr);

    const auto* binary = dynamic_cast<const BinaryExpression*>(assignment->value_expr.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->expr_operator, "+");

    const auto* left = dynamic_cast<const LiteralExpression*>(binary->expr_left.get());
    const auto* right = dynamic_cast<const LiteralExpression*>(binary->expr_right.get());

    ASSERT_NE(left, nullptr);
    ASSERT_NE(right, nullptr);

    EXPECT_EQ(left->value, "2");
    EXPECT_EQ(right->value, "3");
}

TEST(ParserExpressionTest, RespectsOperatorPrecedence)
{
    const std::string code = R"(x = 2 + 3 * 4;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(assignment, nullptr);

    const auto* root = dynamic_cast<const BinaryExpression*>(assignment->value_expr.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->expr_operator, "+");

    const auto* left = dynamic_cast<const LiteralExpression*>(root->expr_left.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->value, "2");

    const auto* right = dynamic_cast<const BinaryExpression*>(root->expr_right.get());
    ASSERT_NE(right, nullptr);

    EXPECT_EQ(right->expr_operator, "*");

    const auto* right_left = dynamic_cast<const LiteralExpression*>(right->expr_left.get());
    const auto* right_right = dynamic_cast<const LiteralExpression*>(right->expr_right.get());

    ASSERT_NE(right_left, nullptr);
    ASSERT_NE(right_right, nullptr);

    EXPECT_EQ(right_left->value, "3");
    EXPECT_EQ(right_right->value, "4");
}

TEST(ParserExpressionTest, RespectsParentheses)
{
    const std::string code = R"(x = (2 + 3) * 4;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(assignment, nullptr);

    const auto* root = dynamic_cast<const BinaryExpression*>(assignment->value_expr.get());

    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->expr_operator, "*");

    const auto* left = dynamic_cast<const BinaryExpression*>(root->expr_left.get());

    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->expr_operator, "+");
}

TEST(ParserExpressionTest, ParsesUnaryExpression)
{
    const std::string code = R"(x = -42;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(assignment, nullptr);

    const auto* unary = dynamic_cast<const UnaryExpression*>(assignment->value_expr.get());

    ASSERT_NE(unary, nullptr);
    EXPECT_EQ(unary->unary_op, "-");

    const auto* child = dynamic_cast<const LiteralExpression*>(unary->child.get());

    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->value, "42");
}

TEST(ParserExpressionTest, ParsesCompoundAddAssignment)
{
    const std::string code = R"(x += 5;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(assignment, nullptr);

    const auto* binary = dynamic_cast<const BinaryExpression*>(assignment->value_expr.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->expr_operator, "+");

    const auto* left = dynamic_cast<const VariableExpression*>(binary->expr_left.get());
    const auto* right = dynamic_cast<const LiteralExpression*>(binary->expr_right.get());

    ASSERT_NE(left, nullptr);
    ASSERT_NE(right, nullptr);

    EXPECT_EQ(left->name, "x");
    EXPECT_EQ(right->value, "5");
}

TEST(ParserExpressionTest, ParsesCompoundSubtractAssignment)
{
    const std::string code = R"(x -= 5;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(assignment, nullptr);

    const auto* binary = dynamic_cast<const BinaryExpression*>(assignment->value_expr.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->expr_operator, "-");
}

TEST(ParserExpressionTest, ParsesCompoundMultiplyAssignment)
{
    const std::string code = R"(x *= 5;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(assignment, nullptr);

    const auto* binary = dynamic_cast<const BinaryExpression*>(assignment->value_expr.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->expr_operator, "*");
}

TEST(ParserExpressionTest, ParsesCompoundDivideAssignment)
{
    const std::string code = R"(x /= 5;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* assignment = dynamic_cast<const AssignmentNode*>(ast[0].get());
    ASSERT_NE(assignment, nullptr);

    const auto* binary = dynamic_cast<const BinaryExpression*>(assignment->value_expr.get());

    ASSERT_NE(binary, nullptr);
    EXPECT_EQ(binary->expr_operator, "/");
}

TEST(ParserExpressionTest, ParsesFunctionCallWithNoArguments)
{
    const std::string code = R"(foo();)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const FunctionCallNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->name, "foo");

    ASSERT_NE(node->expr, nullptr);
    EXPECT_EQ(node->expr->name, "foo");
    EXPECT_TRUE(node->expr->args.empty());
}

TEST(ParserExpressionTest, ParsesFunctionCallWithArguments)
{
    const std::string code = R"(foo(1, 2);)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const FunctionCallNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->expr, nullptr);

    EXPECT_EQ(node->expr->name, "foo");
    ASSERT_EQ(node->expr->args.size(), 2);

    const auto* first = dynamic_cast<const LiteralExpression*>(node->expr->args[0].get());
    const auto* second = dynamic_cast<const LiteralExpression*>(node->expr->args[1].get());

    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);

    EXPECT_EQ(first->value, "1");
    EXPECT_EQ(second->value, "2");
}

TEST(ParserExpressionTest, ParsesFunctionCallExpression)
{
    const std::string code = R"(int x = foo(42);)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* declaration = dynamic_cast<const DeclarationNode*>(ast[0].get());
    ASSERT_NE(declaration, nullptr);

    const auto* call = dynamic_cast<const FunctionCallExpression*>(declaration->initializer.get());

    ASSERT_NE(call, nullptr);
    EXPECT_EQ(call->name, "foo");

    ASSERT_EQ(call->args.size(), 1);

    const auto* argument = dynamic_cast<const LiteralExpression*>(call->args[0].get());

    ASSERT_NE(argument, nullptr);
    EXPECT_EQ(argument->value, "42");
}

TEST(ParserExpressionTest, ParsesNestedFunctionCallExpression)
{
    const std::string code = R"(int x = foo(bar(42));)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* declaration = dynamic_cast<const DeclarationNode*>(ast[0].get());
    ASSERT_NE(declaration, nullptr);

    const auto* outer = dynamic_cast<const FunctionCallExpression*>(declaration->initializer.get());

    ASSERT_NE(outer, nullptr);
    EXPECT_EQ(outer->name, "foo");
    ASSERT_EQ(outer->args.size(), 1);

    const auto* inner = dynamic_cast<const FunctionCallExpression*>(outer->args[0].get());

    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->name, "bar");
    ASSERT_EQ(inner->args.size(), 1);

    const auto* argument = dynamic_cast<const LiteralExpression*>(inner->args[0].get());

    ASSERT_NE(argument, nullptr);
    EXPECT_EQ(argument->value, "42");
}

TEST(ParserExpressionTest, ParsesReturnExpression)
{
    const std::string code = R"(return 2 + 3;)";
    Parser parser(code);
    parser.tokenizeProgram();

    const auto ast = parser.parseProgram(PARSING_MODE::DEFAULT);

    ASSERT_EQ(ast.size(), 1);

    const auto* node = dynamic_cast<const ReturnNode*>(ast[0].get());

    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->ret_expr, nullptr);

    const auto* expression = dynamic_cast<const BinaryExpression*>(node->ret_expr.get());

    ASSERT_NE(expression, nullptr);
    EXPECT_EQ(expression->expr_operator, "+");
}