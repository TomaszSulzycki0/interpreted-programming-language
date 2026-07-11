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

class Parser
{
private:
    std::string code;
    Tokenizer tokenizer;

    std::vector<Token> tokens {};
    std::size_t tokens_size {};
    std::size_t pos {};

    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<Expression> parseAtomicExpression(); 
    std::unique_ptr<Expression> parseRPN(); 

    Token peek() const { return pos >= tokens_size ? Token{ TOKEN_TYPE::TOKEN_EOF } : tokens[pos]; }
    Token advance() { return tokens[pos++]; }
    bool check(TOKEN_TYPE type) const { return peek().type == type; }

    Token consume(TOKEN_TYPE type, std::string_view error_message) 
    {
        if ( check(type) ) 
        { 
            return advance();
        }
        throw std::runtime_error( std::string(error_message) );
    }

public:
    std::vector<std::unique_ptr<ASTNode>> parseProgram(); 
    void tokenizeProgram();
    std::size_t getNumTokens() const { return tokens.size(); }
    explicit Parser(std::string _code) : code(std::move(_code)), tokenizer(code) {}
};



#endif