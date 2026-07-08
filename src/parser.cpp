#include<iostream>

#include"parser.hpp"
#include"numericVariable.hpp"
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
    static const std::regex literal_pattern(R"(^-?\d+(?:\.\d+)?\s*$)");
    static const std::regex variable_pattern(R"(^[a-zA-Z_]\w*$)");
    static const std::regex binary_pattern(R"(^(\S+)\s*([+-/*])\s*(\S+)$)");
    std::smatch matches;
    
    if (expr_str.front() == '"' && expr_str.back() == '"')
    {
        return std::make_unique<LiteralExpression>(expr_str);
    }

    if (std::regex_match(expr_str, literal_pattern)) 
    {
        return std::make_unique<LiteralExpression>(expr_str);
    }

    if (std::regex_match(expr_str, variable_pattern)) 
    {
        return std::make_unique<VariableExpression>(expr_str);
    }

    if (std::regex_match(expr_str, matches, binary_pattern))
    {
        std::string expr_str_left = matches[1];
        std::string expr_str_op = matches[2];
        std::string expr_str_right = matches[3];

        std::unique_ptr<Expression> expr_left = parseInitializer(expr_str_left);
        std::unique_ptr<Expression> expr_right = parseInitializer(expr_str_right);

        if ( !expr_left || !expr_right )
        {
            return nullptr;
        }

        return std::make_unique<BinaryExpression>(expr_str_op, std::move(expr_left), std::move(expr_right));
    }
    
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
    static const std::regex declaration_pattern(R"(([a-zA-Z]+)\s+([a-zA-Z_]\w*)(?:\s*=\s*(\S+.*))?)");
    static const std::regex assignment_pattern(R"(([a-zA-Z_]\w*)\s*=\s*(\S+.*))");
    std::smatch matches;

    if (std::regex_match(statement, matches, declaration_pattern)) 
    {
        std::string type_token = matches[1];
        std::string name_token = matches[2];
        std::unique_ptr<Expression> expr_token = nullptr;

        if (matches[3].matched) 
        {
            std::string expression_token = matches[3];
            expr_token = parseInitializer(expression_token);
            if (!expr_token)
            {
                throw std::runtime_error("Error: Could not parse expression: " + expression_token);
            }
        }

        return std::make_unique<DeclarationNode>(type_token, name_token, std::move(expr_token));
    } 
    else if (std::regex_match(statement, matches, assignment_pattern)) 
    {
        std::string name_token = matches[1];
        std::string expression_token = matches[2];

        std::unique_ptr<Expression> expr_token = parseInitializer(expression_token);

        if (!expr_token)
        {
            throw std::runtime_error("Error: Could not parse expression: " + expression_token);
        }

        return std::make_unique<AssignmentNode>(name_token, std::move(expr_token));
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
    enum class STATE{
        CODE, COMMENT, STRING 
    };

    std::vector<std::string> output {};
    auto code_size = code.size();

    STATE state = STATE::CODE;

    std::string statement {};

    for(std::size_t i {}; i < code_size; ++i)
    {
        char current = code[i];

        if (current == '#' && state != STATE::STRING)
        {
            state = STATE::COMMENT;
            continue;
        } 
        if (current == '\n' && state == STATE::COMMENT)
        {
            state = STATE::CODE;
            continue;
        }
        else if (current == '"' && state == STATE::CODE)
        {
            state = STATE::STRING;
        }
        else if (current == '"' && state == STATE::STRING)
        {
            state = STATE::CODE;
        }
        else if (current == ';' && state == STATE::CODE)
        {
            auto first_char = statement.find_first_not_of(" \n");
            if(statement.empty() || first_char == std::string::npos)
            {
                continue;
            }
            output.push_back(statement.substr(first_char));
            statement.clear();
            continue;
        }

        if (state == STATE::COMMENT)
        {
            continue;
        }

        statement.push_back(current);
    }

    return output;
}