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
    virtual double asDouble() const = 0;
    virtual long long asInteger() const = 0;

    void accept(DeclarationVisitor& visitor) const override 
    {
        visitor.visit(*this);
    }

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
            } else 
            {
                throw std::runtime_error("Error: Cannot assign non-numeric value to numeric variable '" + this->getName() + "'.");
            }
        }, val);
    }

    bool isFloatingPoint() const override 
    { 
        return std::is_floating_point_v<T>; 
    }

    double asDouble() const override 
    { 
        return static_cast<double>(value); 
    }

    long long asInteger() const override 
    { 
        return static_cast<long long>(value); 
    }

    explicit Numeric(std::string _name, const T& _value) : NumericDeclaration(std::move(_name)), value(_value) {}

    template<Arithmetic U>
    explicit Numeric(std::string _name, const Numeric<U>& other) : NumericDeclaration(std::move(_name)), value(static_cast<T>(other.getValue())) {}

    template<Arithmetic U> 
    friend auto operator+(const Numeric<T>& lhs, const Numeric<U>& rhs) 
    {    
        return lhs.getValue() + rhs.getValue();
    }

    template<Arithmetic U>
    Numeric<T>& operator=(const Numeric<U>& other) 
    {    
        this->value = static_cast<T>(other.getValue()); 
        return *this; 
    }

    template<Arithmetic U>
    Numeric<T>& operator=(const U& other) 
    {    
        this->value = static_cast<T>(other); 
        return *this; 
    }
    
};



#endif