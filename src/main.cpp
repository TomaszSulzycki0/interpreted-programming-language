#include<vector>
#include<iostream>
#include<regex>
#include<string>
#include<filesystem>
#include<fstream>
#include<stdexcept>

#include"scope.hpp"

constexpr int num_args_for_default_usage = 2;
constexpr int num_args_for_description = 1;

void programDesc();
bool hasCorrectExtension(const std::string& filename, const std::string& expected_ext);
std::string readFileToString(const std::string& filename);
std::vector<std::string> getStatements(const std::string& code);

int main(int argc, char** argv)
{
    switch (argc)
    {
    case num_args_for_description:
        programDesc();
        return 0;
    case num_args_for_default_usage:
        break;
    default:
        std::cerr << "Invalid number of parameters passed to interpreter: " << argc - 1 << std::endl;
        return -1;
    }

    std::string input_filename = argv[1];

    if(!hasCorrectExtension(input_filename, ".ipl"))
    {
        std::cerr << "Invalid input file extension. Expecting .ipl" << std::endl;
        return -1;
    }

    try
    {
        auto file_content = readFileToString(input_filename);
        auto statements = getStatements(file_content);

        std::cout << "Num statements: " << statements.size() << std::endl;

        for(int i = 0; i < statements.size(); ++i)
        {
            std::cout << "Statement " << i << " " << statements[i] << std::endl;
        } 
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}

void programDesc()
{
    std::cout << "-----------------------\nInterpreted Programming Language\nPersonal Project\nUsage command: ilp filename.ilp\n-----------------------" << std::endl;
}

bool hasCorrectExtension(const std::string& filename, const std::string& expected_ext) 
{
    return std::filesystem::path(filename).extension() == expected_ext;
}

std::string readFileToString(const std::string& filename) 
{
    std::ifstream file_stream(filename);
    if (!file_stream.is_open()) {
        throw std::runtime_error("Unable to access file: " + filename);
    }
    
    std::ostringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

std::vector<std::string> getStatements(const std::string& code)
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
        if(raw_substring.empty() || first_char  == std::string::npos)
        {
            continue;
        }

        auto statement = raw_substring.substr(first_char);
        output.push_back(statement);
    }

    return output;
}