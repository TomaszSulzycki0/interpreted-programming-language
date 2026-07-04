#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include<concepts>
#include<string>

template<typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

class Declaration
{
private:
    std::string name;
protected:
    Declaration(const std::string& _name) : name(_name) {}
public:
    std::string getName() const { return name; }
    virtual ~Declaration() = default;

};

template<Arithmetic T> 
class Numeric : public Declaration
{
private:
    T value;
public:
    T getValue() const { return value; }
    void setValue(const T& _value) { value = _value; }

    Numeric(const std::string& _name) : Declaration(_name), value(0) {}
    Numeric(const std::string& _name, const T& _value) : Declaration(_name), value(_value) {}

    template<Arithmetic U>
    Numeric(const std::string& _name, const Numeric<U>& other) : Declaration(_name), value(static_cast<T>(other.getValue())) {}

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