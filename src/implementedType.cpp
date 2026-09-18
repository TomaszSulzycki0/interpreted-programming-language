#include "implementedType.hpp"

ImplementedType to_type(const std::string& str)
{
    if ( str == "bool" )        return ImplementedType::_bool;
    if ( str == "int" )         return ImplementedType::_int;
    if ( str == "float" )       return ImplementedType::_float;
    if ( str == "double" )      return ImplementedType::_double;
    if ( str == "string" )      return ImplementedType::_string;
    if ( str == "void" )        return ImplementedType::_void;
    if ( str == "fn" )          return ImplementedType::_fn;
    if ( str == "NULL_TYPE" )   return ImplementedType::NULL_TYPE;

    return ImplementedType::NULL_TYPE;
}

std::string to_string(ImplementedType tp)
{
    switch (tp) 
    {
        case ImplementedType::_bool:        return "bool";
        case ImplementedType::_int:         return "int";
        case ImplementedType::_float:       return "float";
        case ImplementedType::_double:      return "double";
        case ImplementedType::_string:      return "string";
        case ImplementedType::_void:        return "void";
        case ImplementedType::_fn:          return "fn";
        case ImplementedType::NULL_TYPE:    return "NULL_TYPE";
        default:                            return "NULL_TYPE";
    }
}

std::string operator+(std::string lhs, ImplementedType rhs)
{
    return lhs + to_string(rhs);
}

std::string operator+(ImplementedType lhs, std::string rhs) 
{
    return to_string(lhs) + rhs;
}