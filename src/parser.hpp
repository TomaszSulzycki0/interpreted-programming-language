#ifndef PARSER_HPP
#define PARSER_HPP

#include<memory>
#include<map>
#include<functional>
#include<optional>
#include<regex>
#include<fstream>

#include"expression.hpp"

// Raw data for builder
//
struct DeclarationNode 
{
    std::string type;
    std::string name;
    std::unique_ptr<Expression> initializer;

    DeclarationNode(std::string t, std::string n, std::unique_ptr<Expression> init)
        : type(std::move(t)), name(std::move(n)), initializer(std::move(init)) {}
};

class Parser
{
private:
    std::unique_ptr<Expression> parseInitializer(const std::string& expr_str); 
    std::unique_ptr<DeclarationNode> parseStatement(const std::string& statement);

public:
    std::vector<std::unique_ptr<DeclarationNode>> parseProgram(const std::vector<std::string>& statements); 
    std::vector<std::string> getStatements(const std::string& code);
    std::string readFileToString(const std::string& filename); 

};



#endif