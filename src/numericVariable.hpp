#ifndef NUMERIC_VARIABLE_HPP
#define NUMERIC_VARIABLE_HPP

#include<concepts>

#include"declarationBase.hpp"

template<typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

class NumericDeclaration : public ValueDeclaration
{
public:
    using ValueDeclaration::ValueDeclaration;
    virtual bool isFloatingPoint() const = 0;
    virtual bool isBool() const = 0;
    virtual double asDouble() const = 0;
    virtual bool asBool() const = 0;
    virtual long long asInteger() const = 0;

    void accept(DeclarationVisitor& visitor) const override { visitor.visit(*this); }
};

template<Arithmetic T> 
class Numeric final : public NumericDeclaration
{
private:
    T value;
public:
    RuntimeValue getValue() const override { return value; }
    
    void setValue(const RuntimeValue& val) override 
    {
        std::visit([this](auto&& unpacked_val) {
            using EvaluatedType = std::decay_t<decltype(unpacked_val)>;
            
            if constexpr (std::is_arithmetic_v<EvaluatedType>) 
            {
                this->value = static_cast<T>(unpacked_val);
            } 
            else 
            {
                throw std::runtime_error("Error: Cannot assign non-numeric value to numeric variable '" + std::string( this->getName() ) + "'.");
            }
        }, val);
    }

    std::string getType() const override
    {
        if constexpr ( std::same_as<T, int> )
        {
            return "int";
        }
        else if constexpr ( std::same_as<T, double> )
        {
            return "double";
        }
        else if constexpr ( std::same_as<T, float> )
        {
            return "float";
        }
        else if constexpr ( std::same_as<T, bool> )
        {
            return "bool";
        }
        else
        {
            static_assert(!std::same_as<T, T>, "Error: Invalid numeric type.");
        }
    }

    bool isFloatingPoint() const override 
    { 
        return std::is_floating_point_v<T>; 
    }

    bool isBool() const override 
    { 
        return std::is_same_v<T, bool>;
    }

    bool asBool() const override
    {
        return static_cast<bool>(value);
    }

    double asDouble() const override 
    { 
        return static_cast<double>(value); 
    }

    long long asInteger() const override 
    { 
        return static_cast<long long>(value); 
    }

    explicit Numeric(std::string_view _name, const T& _value) : NumericDeclaration(_name), value(_value) {}
};



#endif