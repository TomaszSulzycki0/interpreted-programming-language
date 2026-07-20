#ifndef PARSER_HPP
#define PARSER_HPP

#include<memory>
#include<map>
#include<functional>
#include<optional>
#include<regex>
#include<fstream>

#include"expression.hpp"
#include"tokenizer.hpp"

class NodeVisitor;

class ASTNode
{
public:
    virtual void accept(NodeVisitor& visitor) const = 0;
    virtual ~ASTNode() = default;
};

class AssignmentNode : public ASTNode 
{
public:
    std::string name;                        
    std::unique_ptr<Expression> value_expr; 

    void accept(NodeVisitor& visitor) const override;
    explicit AssignmentNode(std::string n, std::unique_ptr<Expression> val)
        : name(std::move(n)), value_expr(std::move(val)) {}
};

class DeclarationNode : public ASTNode
{
public:
    std::string type;
    std::string name;
    std::unique_ptr<Expression> initializer;

    void accept(NodeVisitor& visitor) const override;
    explicit DeclarationNode(std::string t, std::string n, std::unique_ptr<Expression> init)
        : type(std::move(t)), name(std::move(n)), initializer(std::move(init)) {}
};

class FunctionCallNode : public ASTNode
{
public:
    std::string name;
    std::vector<std::string> args;
    void accept(NodeVisitor& visitor) const override;
};

class FunctionDeclarationNode : public ASTNode
{
public:
    std::string return_type;
    std::string name;
    std::vector<std::unique_ptr<DeclarationNode>> arg_nodes;
    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    std::unique_ptr<Expression> return_expr;

    void accept(NodeVisitor& visitor) const override;

    explicit FunctionDeclarationNode(
        std::string rt, 
        std::string n, 
        std::vector<std::unique_ptr<DeclarationNode>> _args,
        std::vector<std::unique_ptr<ASTNode>> bdnds,
        std::unique_ptr<Expression> ret_expr) :
            return_type(std::move(rt)), 
            name(std::move(n)), 
            arg_nodes(std::move(_args)),
            body_nodes(std::move(bdnds)),
            return_expr(std::move(ret_expr)) {}

};

class Parser
{
private:
    std::string code;
    Tokenizer tokenizer;

    std::vector<Token> tokens {};
    std::size_t tokens_size {};
    std::size_t pos {};

    bool is_good_for_exec = true;

    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<ASTNode> parseFunctionDeclaration();
    std::vector<std::unique_ptr<ASTNode>> parseFunctionBody();
    std::unique_ptr<Expression> parseAtomicExpression(); 
    std::unique_ptr<Expression> parseRPN(); 

    // Return token at current pos
    Token peek() const { return pos >= tokens_size ? Token{ TOKEN_TYPE::TOKEN_EOF } : tokens[pos]; }

    // Return token at current pos and move one token forward
    Token advance() { return tokens[pos++]; }

    // Check if the type of the token at current pos in the same as arg
    bool check(TOKEN_TYPE type) const { return peek().type == type; }

    // If the type of the token at current pos is the same as arg1 - move one token forward
    // else throw with error_message
    Token consume(TOKEN_TYPE type, std::string_view error_message) 
    {
        if ( check(type) ) 
        { 
            return advance();
        }
        throw std::runtime_error( std::string(error_message) );
    }
            
    bool isAtomicExpr(Token t) const
    {
        return  t.type == TOKEN_TYPE::TOKEN_IDENTIFIER ||
                t.type == TOKEN_TYPE::TOKEN_LITERAL_FLOAT ||
                t.type == TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL ||
                t.type == TOKEN_TYPE::TOKEN_STRING_START ||
                t.type == TOKEN_TYPE::TOKEN_KEYWORD_BOOL;     
    }

    void makeBinExprRPN(std::vector<Token>& operator_stack, std::vector<std::unique_ptr<Expression>>& expr_stack);
            
public:
    // Generate AST from the tokens
    std::vector<std::unique_ptr<ASTNode>> parseProgram(); 

    // Use Tokenizer to emit tokens
    void tokenizeProgram();

    std::size_t getNumTokens() const { return tokens.size(); }

    // If parser throws at any point, 
    // it continues parsing but marks itself as non executable
    bool isASTExecutable() const { return is_good_for_exec; }

    explicit Parser(std::string _code) : code(std::move(_code)), tokenizer(code) {}
};

#endif