#ifndef PARSER_HPP
#define PARSER_HPP

#include<memory>
#include<map>
#include<functional>
#include<optional>
#include<regex>
#include<fstream>

#include"variable.hpp"

struct NumericInitArgs 
{
    std::string name;
    std::optional<std::string> value; 
};

class Parser
{
private:
    std::map<std::string, std::function<std::unique_ptr<NumericDeclaration>(const NumericInitArgs&)>> num_innit_map;
    // fix: Parser should only parse. Separate syntax validation from object creation.

public:
    std::unique_ptr<NumericDeclaration> parse(const std::string& statement);
    std::vector<std::string> getStatements(const std::string& code);
    std::string readFileToString(const std::string& filename); 

    Parser();

};



#endif