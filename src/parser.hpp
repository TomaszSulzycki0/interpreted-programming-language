#ifndef PARSER_HPP
#define PARSER_HPP

#include<memory>
#include<map>
#include<functional>
#include<optional>
#include<regex>
#include<fstream>

#include"expression.hpp"

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
    std::unique_ptr<Expression> parseInitializer(const std::string& expr_str); 
    std::unique_ptr<ASTNode> parseStatement(const std::string& statement);

public:
    std::vector<std::unique_ptr<ASTNode>> parseProgram(const std::vector<std::string>& statements); 
    std::vector<std::string> getStatements(const std::string& code);
    std::string readFileToString(const std::string& filename); 
};



#endif