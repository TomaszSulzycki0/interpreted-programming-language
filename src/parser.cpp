#include<iostream>

#include"parser.hpp"
#include"variable.hpp"
#include"expression.hpp"

void AssignmentNode::accept(NodeVisitor& visitor) const 
{
    visitor.visit(*this); 
}

void DeclarationNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this); 
}

std::unique_ptr<Expression> Parser::parseInitializer(const std::string& expr_str) 
{
    static const std::regex literal_pattern(R"(^-?\d+(?:\.\d+)?$)");
    if (std::regex_match(expr_str, literal_pattern)) 
    {
        return std::make_unique<LiteralExpression>(expr_str);
    }

    static const std::regex variable_pattern(R"(^[a-zA-Z_]\w*$)");
    if (std::regex_match(expr_str, variable_pattern)) 
    {
        return std::make_unique<VariableExpression>(expr_str);
    }

    // Future Expansion: Complex expressions like "x + 5" 
    return nullptr; 
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseProgram(const std::vector<std::string>& statements) 
{
    std::vector<std::unique_ptr<ASTNode>> ast;

    for (const auto& statement : statements) 
    {
        auto parsed_node = parseStatement(statement); 
        
        if (parsed_node == nullptr) 
        {            
            throw std::runtime_error("Error: Could not parse statement: " + statement); 
        }
        
        ast.push_back(std::move(parsed_node));
    }

    return ast; 
}

std::unique_ptr<ASTNode> Parser::parseStatement(const std::string& statement)
{
    static const std::regex declaration_pattern(R"(([a-zA-Z]+)\s+(\w+)(?:\s*=\s*(\S+.*))?)");
    std::smatch matches;

    if (std::regex_match(statement, matches, declaration_pattern)) 
    {
        std::string type_token = matches[1];
        std::string name_token = matches[2];
        std::unique_ptr<Expression> expr_token = nullptr;

        if (matches[3].matched) 
        {
            expr_token = parseInitializer(matches[3].str());
        }

        return std::make_unique<DeclarationNode>(type_token, name_token, std::move(expr_token));
    }

    return nullptr;
}

std::string Parser::readFileToString(const std::string& filename) 
{
    std::ifstream file_stream(filename);
    if (!file_stream.is_open()) 
    {
        throw std::runtime_error("Unable to access file: " + filename);
    }
    
    std::ostringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

std::vector<std::string> Parser::getStatements(const std::string& code)
{
    std::vector<std::string> output;
    std::size_t pos {};
    auto code_size = code.size();
    
    for(std::size_t i {}; i < code_size; ++i)
    {
        auto semicolon_pos = code.find(';', pos);

        // No next semicolon
        if(semicolon_pos  == std::string::npos)
        {
            break;
        }

        // Does not include semicolon in statement
        auto raw_substring = code.substr(pos, semicolon_pos - pos);
        auto first_char = raw_substring.find_first_not_of(" \n");

        pos = semicolon_pos + 1; 

        // Edge cases
        // Handles multiple semicolons, white spaces, new lines
        // fix: WILL NOT THROW IF THE LAST LINE OF CODE WITHOUT SEMICOLON!
        if(raw_substring.empty() || first_char == std::string::npos)
        {
            continue;
        }

        auto statement = raw_substring.substr(first_char);
        output.push_back(statement);
    }

    return output;
}