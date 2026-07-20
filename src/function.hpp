#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include"declarationBase.hpp"

class Function final : public FunctionDeclaration
{
private:
public:
    RuntimeValue call(const std::vector<RuntimeValue> _args) override;
};

#endif