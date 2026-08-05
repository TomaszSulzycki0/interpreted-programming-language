#include<vector>
#include<iostream>
#include<string>
#include<filesystem>
#include<stdexcept>
#include<fstream>

#include"scope.hpp"
#include"numericVariable.hpp"
#include"parser.hpp"
#include"visitors.hpp"
#include"builder.hpp"
#include"typeChecker.hpp"

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
        std::cerr << "Error: Invalid number of parameters passed to interpreter: " << argc - 1 << std::endl;
        return -1;
    }

    std::string input_filename = argv[1];

    if( !hasCorrectExtension(input_filename, ".ipl") )
    {
        std::cerr << "Error: Invalid input file extension. Expecting .ipl" << std::endl;
        return -1;
    }

    try
    {
        std::cout << "Evaluating: " + input_filename << std::endl;
        Parser parser { readFileToString(input_filename) };

        SemanticScope main_semantic_scope {};
        TypeChecker type_checker {main_semantic_scope};
        
        Scope main_scope {};
        Builder builder {main_scope};
        
        std::cout << "Lexing.." << std::endl;
        parser.tokenizeProgram();
        
        std::cout << "Captured: " << parser.getNumTokens() << " tokens." << std::endl;

        std::cout << "Parsing.." << std::endl;
        auto program_ast = parser.parseProgram( PARSING_MODE::DEFAULT );

        if ( parser.isASTExecutable() )
        {
            std::cout << "Checking types.." << std::endl;

            type_checker.run_check( program_ast );

            std::cout << "Executing.." << std::endl;
            builder.buildProgram( program_ast ); 
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
    if (!file_stream.is_open()) 
    {
        throw std::runtime_error("Error: Unable to access file: \'" + filename + "\'.");
    }
    
    std::ostringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}