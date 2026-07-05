#include"parser.hpp"

std::unique_ptr<NumericDeclaration> Parser::parse(const std::string& statement)
{
    static const std::regex declaration_pattern(R"(([a-zA-Z]+)\s+(\w+)(?:\s*=\s*(.+))?)");

    std::smatch matches;

    if (std::regex_match(statement, matches, declaration_pattern)) 
    {
        std::string type_token = matches[1];
        std::string name_token = matches[2];

        auto it = num_innit_map.find(type_token);
        if (it != num_innit_map.end()) 
        {
            NumericInitArgs args;
            args.name = name_token;
            if (matches[3].matched) 
            {
                args.value = matches[3].str();
            }
            return it->second(args);
        }
    }

    return nullptr;
}

Parser::Parser()
{
    num_innit_map["i"] = [](const NumericInitArgs& args) 
    {
        if (args.value.has_value()) 
        {
            int parsed_val = std::stoi(args.value.value());
            return std::make_unique<Numeric<int>>(args.name, parsed_val);
        }
        return std::make_unique<Numeric<int>>(args.name);
    };

    num_innit_map["d"] = [](const NumericInitArgs& args) 
    {
        if (args.value.has_value()) 
        {
            double parsed_val = std::stod(args.value.value()); 
            return std::make_unique<Numeric<double>>(args.name, parsed_val);
        }
        return std::make_unique<Numeric<double>>(args.name);
    };

    num_innit_map["f"] = [](const NumericInitArgs& args) 
    {
        if (args.value.has_value()) 
        {
            float parsed_val = std::stof(args.value.value()); 
            return std::make_unique<Numeric<float>>(args.name, parsed_val);
        }
        return std::make_unique<Numeric<float>>(args.name);
    };
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