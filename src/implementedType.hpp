#ifndef IMPLEMENTED_TYPE_HPP
#define IMPLEMENTED_TYPE_HPP

#include<string>

enum class ImplementedType
{
    _bool, _int, _float, _double, _string, _void, _fn, NULL_TYPE
};

ImplementedType to_type(const std::string& str);
std::string to_string(ImplementedType tp);
std::string operator+(std::string lhs, ImplementedType rhs); 
std::string operator+(ImplementedType lhs, std::string rhs); 

#endif