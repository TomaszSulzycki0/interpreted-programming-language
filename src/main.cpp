#include<iostream>

int main(std::size_t argc, char** argv)
{
    for(std::size_t i {}; i < argc; ++i)
    {
        std::printf(argv[i]);
        std::printf("\n");
    }

    std::printf("test.\n");
    int in = 0;
    std::cin >> in;
}