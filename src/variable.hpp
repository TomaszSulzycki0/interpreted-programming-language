#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include<concepts>

#include"declarationBase.hpp"

template<typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

template<Arithmetic T> 
class Numeric : public NumericDeclaration
{
private:
    T value;
public:
    T getValue() const { return value; }
    void setValue(const T& _value) { value = _value; }

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

    Numeric(const std::string& _name) : NumericDeclaration(_name), value(0) {}
    Numeric(const std::string& _name, const T& _value) : NumericDeclaration(_name), value(_value) {}

    template<Arithmetic U>
    Numeric(const std::string& _name, const Numeric<U>& other) : NumericDeclaration(_name), value(static_cast<T>(other.getValue())) {}

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