#include <gtest/gtest.h>

#include "tokenizer.hpp"

#include <string>
#include <vector>

namespace
{

struct ExpectedToken
{
    TOKEN_TYPE type;
    std::string value;
    std::size_t line;
};

void expectTokens(const std::vector<Token>& actual, const std::vector<ExpectedToken>& expected)
{
    ASSERT_EQ(actual.size(), expected.size());

    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        EXPECT_EQ(actual[i].type, expected[i].type)
            << "Unexpected token type at index " << i;

        EXPECT_EQ(actual[i].value, expected[i].value)
            << "Unexpected token value at index " << i;

        EXPECT_EQ(actual[i].line, expected[i].line)
            << "Unexpected line at index " << i;
    }
}

} 

TEST(TokenizerTest, EmptyInput)
{
    const std::string code = R"()";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, TOKEN_TYPE::TOKEN_EOF);
}


TEST(TokenizerTest, WhiteSpaceInput)
{
    const std::string code = R"(   )";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, TOKEN_TYPE::TOKEN_EOF);
}

TEST(TokenizerTest, Identifier)
{
    const std::string code = R"(foo)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "foo", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, MultipleIdentifiers)
{
    const std::string code = R"(foo bar baz)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "foo", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "bar", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "baz", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, IdentifierWithUnderscore)
{
    const std::string code = R"(num_samples)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "num_samples", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, FunctionIdentifier)
{
    const std::string code = R"(foo())";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, "foo", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}

TEST(TokenizerTest, FunctionCallWithArguments)
{
    const std::string code = R"(foo(x, y))";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, "foo", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "x", 1},
        {TOKEN_TYPE::TOKEN_COMMA, ",", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "y", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, IdentifierIsNotFunctionIdentifierWithoutCall)
{
    const std::string code = R"(foo)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].type, TOKEN_TYPE::TOKEN_IDENTIFIER);
    EXPECT_EQ(tokens[0].value, "foo");
}

TEST(TokenizerTest, TypeKeywords)
{
    const std::string code = R"(int float double bool void string)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "float", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "double", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "bool", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "void", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "string", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, LanguageKeywords)
{
    const std::string code = R"(fn true false if else while return)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_KEYWORD_FUNCTION, "fn", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_BOOL, "true", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_BOOL, "false", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_IF, "if", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_ELSE, "else", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_WHILE, "while", 1},
        {TOKEN_TYPE::TOKEN_RETURN, "return", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}

TEST(TokenizerTest, LogicalOperators)
{
    const std::string code = R"(and or)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_OPERATOR, "and", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "or", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, IntegralLiterals)
{
    const std::string code = R"(0 1 42 100 123456)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, "0", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, "1", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, "42", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, "100", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, "123456", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, FloatLiterals)
{
    const std::string code = R"(0.0 1.5 3.14159 100.25)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_LITERAL_FLOAT, "0.0", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_FLOAT, "1.5", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_FLOAT, "3.14159", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_FLOAT, "100.25", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}

TEST(TokenizerTest, Punctuation)
{
    const std::string code = R"((){};,)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_BRACE_OPEN, "{", 1},
        {TOKEN_TYPE::TOKEN_BRACE_CLOSE, "}", 1},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 1},
        {TOKEN_TYPE::TOKEN_COMMA, ",", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}

TEST(TokenizerTest, Arrow)
{
    const std::string code = R"(->)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_ARROW, "->", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, AssignmentOperators)
{
    const std::string code = R"(= += -= *= /=)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_EQUALS, "=", 1},
        {TOKEN_TYPE::TOKEN_PLUS_EQUALS, "+=", 1},
        {TOKEN_TYPE::TOKEN_MINUS_EQUALS, "-=", 1},
        {TOKEN_TYPE::TOKEN_MUL_EQUALS, "*=", 1},
        {TOKEN_TYPE::TOKEN_DIV_EQUALS, "/=", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, BinaryOperators)
{
    const std::string code = R"(+ - * / % == != < > <= >=)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_OPERATOR, "+", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "-", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "*", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "/", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "%", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "==", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "!=", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "<", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, ">", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "<=", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, ">=", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}

TEST(TokenizerTest, String)
{
    const std::string code = R"("hello world")";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_STRING_START, "\"", 1},
        {TOKEN_TYPE::TOKEN_STRING_BODY, "hello world", 1},
        {TOKEN_TYPE::TOKEN_STRING_END, "\"", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, EmptyString)
{
    const std::string code = R"("")";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_STRING_START, "\"", 1},
        {TOKEN_TYPE::TOKEN_STRING_END, "\"", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, StringWithEscapeSequence)
{
    const std::string code = R"("hello\nworld")";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    ASSERT_EQ(tokens.size(), 4u);

    EXPECT_EQ(tokens[0].type, TOKEN_TYPE::TOKEN_STRING_START);
    EXPECT_EQ(tokens[0].value, "\"");

    EXPECT_EQ(tokens[1].type, TOKEN_TYPE::TOKEN_STRING_BODY);

    EXPECT_EQ(tokens[2].type, TOKEN_TYPE::TOKEN_STRING_END);
    EXPECT_EQ(tokens[2].value, "\"");

    EXPECT_EQ(tokens[3].type, TOKEN_TYPE::TOKEN_EOF);
}


TEST(TokenizerTest, IgnoresWhitespace)
{
    const std::string code = R"(   foo    bar		baz   )";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "foo", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "bar", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "baz", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, TracksLineNumbers)
{
    const std::string code = R"(int x;
int y;
int z;)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "x", 1},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 1},

        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 2},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "y", 2},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 2},

        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 3},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "z", 3},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 3},

        {TOKEN_TYPE::TOKEN_EOF, "", 3}
    });
}


TEST(TokenizerTest, SingleLineComment)
{
    const std::string code = R"(# this is a comment)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    ASSERT_FALSE(tokens.empty());

    EXPECT_EQ(tokens[0].type, TOKEN_TYPE::TOKEN_COMMENT_START);
    EXPECT_EQ(tokens[0].value, "#");

    EXPECT_EQ(tokens.back().type, TOKEN_TYPE::TOKEN_EOF);
}


TEST(TokenizerTest, CommentDoesNotConsumeNextLine)
{
    const std::string code = R"(# comment
int x;)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    auto it = std::find_if(
        tokens.begin(),
        tokens.end(),
        [](const Token& token)
        {
            return token.type == TOKEN_TYPE::TOKEN_KEYWORD_TYPE &&
                   token.value == "int";
        });

    ASSERT_NE(it, tokens.end());
    EXPECT_EQ(it->line, 2);
}


TEST(TokenizerTest, CodeAfterCommentIsTokenized)
{
    const std::string code = R"(# comment
foo())";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    auto it = std::find_if(
        tokens.begin(),
        tokens.end(),
        [](const Token& token)
        {
            return token.value == "foo";
        });

    ASSERT_NE(it, tokens.end());
    EXPECT_EQ(it->type, TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER);
    EXPECT_EQ(it->line, 2);
}


TEST(TokenizerTest, VariableDeclaration)
{
    const std::string code = R"(int num_samples = 100;)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "num_samples", 1},
        {TOKEN_TYPE::TOKEN_EQUALS, "=", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, "100", 1},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, FunctionDeclaration)
{
    const std::string code =
        R"(fn foo(int x, int num_steps) -> int)";

    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_KEYWORD_FUNCTION, "fn", 1},
        {TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, "foo", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "x", 1},
        {TOKEN_TYPE::TOKEN_COMMA, ",", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "num_steps", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_ARROW, "->", 1},
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, IfStatement)
{
    const std::string code = R"(if (x == 1) { return x; })";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_KEYWORD_IF, "if", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "x", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "==", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, "1", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_BRACE_OPEN, "{", 1},
        {TOKEN_TYPE::TOKEN_RETURN, "return", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "x", 1},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 1},
        {TOKEN_TYPE::TOKEN_BRACE_CLOSE, "}", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, WhileStatement)
{
    const std::string code = R"(while (i < num_samples) { i += 1; })";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_KEYWORD_WHILE, "while", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "i", 1},
        {TOKEN_TYPE::TOKEN_OPERATOR, "<", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "num_samples", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_BRACE_OPEN, "{", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "i", 1},
        {TOKEN_TYPE::TOKEN_PLUS_EQUALS, "+=", 1},
        {TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, "1", 1},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 1},
        {TOKEN_TYPE::TOKEN_BRACE_CLOSE, "}", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, NestedFunctionCalls)
{
    const std::string code = R"(foo(bar(x)))";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, "foo", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, "bar", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "x", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, FunctionCallAfterAssignment)
{
    const std::string code = R"(int result = foo(x);)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_KEYWORD_TYPE, "int", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "result", 1},
        {TOKEN_TYPE::TOKEN_EQUALS, "=", 1},
        {TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, "foo", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "x", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 1},
        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, StringConcatenationWithFunctionCalls)
{
    const std::string code =
        R"(print("value: " + string(result) + "\n");)";

    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    expectTokens(tokens,
    {
        {TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, "print", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},

        {TOKEN_TYPE::TOKEN_STRING_START, "\"", 1},
        {TOKEN_TYPE::TOKEN_STRING_BODY, "value: ", 1},
        {TOKEN_TYPE::TOKEN_STRING_END, "\"", 1},

        {TOKEN_TYPE::TOKEN_OPERATOR, "+", 1},

        {TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, "string", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, "(", 1},
        {TOKEN_TYPE::TOKEN_IDENTIFIER, "result", 1},
        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},

        {TOKEN_TYPE::TOKEN_OPERATOR, "+", 1},

        {TOKEN_TYPE::TOKEN_STRING_START, "\"", 1},
        {TOKEN_TYPE::TOKEN_STRING_BODY, "\n", 1},
        {TOKEN_TYPE::TOKEN_STRING_END, "\"", 1},

        {TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, ")", 1},
        {TOKEN_TYPE::TOKEN_SEMICOLON, ";", 1},

        {TOKEN_TYPE::TOKEN_EOF, "", 1}
    });
}


TEST(TokenizerTest, InvalidCharacter)
{
    const std::string code = R"(@)";
    Tokenizer tokenizer(code);

    const auto tokens = tokenizer.emitTokens();

    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(tokens[0].type, TOKEN_TYPE::TOKEN_ERROR);
}