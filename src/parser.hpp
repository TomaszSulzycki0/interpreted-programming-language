#ifndef PARSER_HPP
#define PARSER_HPP

#include<memory>

#include"expression.hpp"
#include"tokenizer.hpp"
#include"abstractNodes.hpp"
#include"RPNHandler.hpp"

class ASTNode;

enum class PARSING_MODE
{
    DEFAULT,
    IF,
    WHILE,  
    FUNCTION_BODY
};

class IParserContext 
{
public:
    virtual ~IParserContext() = default;
    virtual Token advance() = 0;
    virtual Token peek() const = 0;
    virtual std::size_t getNumTokens() const = 0;
    virtual bool isAtomicExpr(Token t) const = 0;
    virtual bool isDone() const = 0;
    virtual bool check(TOKEN_TYPE type) const = 0;
    virtual std::unique_ptr<Expression> parseAtomicExpression() = 0; 
    virtual std::unique_ptr<FunctionCallExpression> parseNonVoidFunctionCall() = 0;

};

class Parser : public IParserContext
{
private:
    std::string code;
    Tokenizer tokenizer;
    RPNHandler rpner;

    std::vector<Token> tokens {};
    std::size_t tokens_size {};
    std::size_t pos {};
    
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<ASTNode> parseFunctionDeclaration();
    std::unique_ptr<ASTNode> parseVoidFunctionCall();

    std::unique_ptr<Expression> parseAtomicExpression(); 
    std::unique_ptr<FunctionCallExpression> parseNonVoidFunctionCall();
    std::unique_ptr<FunctionCallExpression> parseFunctionCallArguments( std::string_view f_identifier ); 

    // If parser throws at any point, 
    // it continues parsing but marks itself as non executable
    bool is_good_for_exec = true;
    
    // Return token at current pos
    Token peek() const { return isDone() ? Token{ TOKEN_TYPE::TOKEN_EOF } : tokens[pos]; }

    // Return token at current pos and move one token forward
    Token advance() { return tokens[pos++]; }

    // Check if the type of the token at current pos in the same as arg
    bool check(TOKEN_TYPE type) const { return peek().type == type; }

    // Pos range check
    bool isDone() const { return pos >= tokens_size; }

    // If the type of the token at current pos is the same as arg1 - move one token forward
    // else throw with error_message
    Token consume(TOKEN_TYPE type, std::string_view error_message);

    bool isAtomicExpr(Token t) const;
            
public:
    // Generate AST from the tokens
    std::vector<std::unique_ptr<ASTNode>> parseProgram(const PARSING_MODE& mode); 

    // Use Tokenizer to emit tokens
    void tokenizeProgram();

    std::size_t getNumTokens() const { return tokens.size(); }

    bool isASTExecutable() const { return is_good_for_exec; }

    explicit Parser(std::string _code) : code(std::move(_code)), tokenizer(code) {}
};

#endif