#ifndef STRING_VARIABLE_HPP
#define STRING_VARIABLE_HPP

#include"declarationBase.hpp"

class StringDeclaration final : public ValueDeclaration
{
private:
    std::string value;
public:

    RuntimeValue getValue() const override { return value; }
    std::string getString() const { return value; }

    void setValue(const RuntimeValue& val) override 
    {
        std::visit([this](auto&& unpacked_val){
            using EvaluatedType = std::decay_t<decltype(unpacked_val)>;
            
            if constexpr (std::same_as<EvaluatedType, std::string>) 
            {
                this->value = unpacked_val;
            } else 
            {
                throw std::runtime_error("Error: Cannot assign non-string value to string variable '" + this->getName() + "'.");
            }
        }, val);
    }

    void accept(DeclarationVisitor& visitor) const override { visitor.visit(*this); }

    explicit StringDeclaration(std::string _name, std::string _value) : ValueDeclaration(_name), value(std::move(_value)) {}


};

#endif