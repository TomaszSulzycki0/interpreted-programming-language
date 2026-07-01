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
        std::cout << file_content;

        Scope global_scope {};
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