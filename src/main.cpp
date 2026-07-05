#include<vector>
#include<iostream>
#include<string>
#include<filesystem>
#include<stdexcept>


#include"scope.hpp"
#include"variable.hpp"
#include"parser.hpp"
#include"visitors.hpp"
#include"builder.hpp"

constexpr int num_args_for_default_usage = 2;
constexpr int num_args_for_description = 1;


void programDesc();
bool hasCorrectExtension(const std::string& filename, const std::string& expected_ext);

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
        Parser parser {};
        Scope main_scope {};
        Builder builder {main_scope};

        auto file_content = parser.readFileToString(input_filename);
        auto statements = parser.getStatements(file_content);
        
        std::cout << "Num statements: " << statements.size() << std::endl;

        for(int i = 0; i < statements.size(); ++i)
        {
            std::cout << "Statement " << i << " " << statements[i] << std::endl;

            auto parsed = parser.parse(statements[i]);

            if (parsed == nullptr) 
            {
                std::cerr << "Syntax Error: Could not parse statement: " << statements[i] << std::endl;
                continue; 
            }

            auto real_declaration = builder.build(*parsed); 
            if (real_declaration == nullptr) 
            {
                std::cerr << "Semantic Error: Builder failed to create node for type: " << parsed->type << std::endl;
                continue; 
            }
            
            std::cout << real_declaration->getName() << " : ";
            PrintVisitor visitor;
            real_declaration->accept(visitor);
            std::cout << std::endl;

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

