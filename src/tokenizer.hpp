#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include<string>
#include<vector>
#include<memory>
#include<set>
#include<regex>
#include<stdexcept>
#include<unordered_map>

enum class TOKENIZER_STATE
{
    NULL_STATE,
    MAKE_TEXT, 
    MAKE_NUMBER,
    MAKE_OPERATOR,
    MAKE_STRING
};

enum class TOKEN_TYPE
{

};

struct Token
{
    TOKEN_TYPE type;
    std::string value {};
};

class Tokenizer
{
private:
    TOKENIZER_STATE current_state {TOKENIZER_STATE::NULL_STATE};
    std::string code;
    const std::size_t code_size;
    static std::unordered_map<TOKENIZER_STATE, std::string> state_regex_map;

    TOKENIZER_STATE assignStartState(char c);
public:
    Tokenizer(std::string _code);
    std::vector<Token> emitTokens();
};

std::vector<Token> Tokenizer::emitTokens()
{
    std::vector<Token> tokens {};
    std::string char_accum {};
    
    for ( std::size_t pos {}; pos < code_size; ++pos )
    {
        char current_char = code[pos];
        auto current_char_str = std::to_string(current_char);

        if ( current_state == TOKENIZER_STATE::NULL_STATE )
        {
            current_state = assignStartState(current_char);
        }

        switch (current_state)
        {
        case TOKENIZER_STATE::MAKE_TEXT:
            
            if ( std::regex_match( current_char_str, std::regex( state_regex_map[current_state] ) ) )
            {
                char_accum.push_back(current_char);
            }
            break;
        case TOKENIZER_STATE::MAKE_NUMBER:
            
            break;
        case TOKENIZER_STATE::MAKE_OPERATOR:
            
            break;
        case TOKENIZER_STATE::MAKE_STRING:
            
            break;
        default:
            break;
        }

    }



    return tokens;
}

TOKENIZER_STATE Tokenizer::assignStartState(char c)
{
    // Loop over map instead
    static const std::regex text_pattern(R"(^[a-zA-Z_]$)");
    static const std::regex digit_pattern(R"(^[0-9]$)");

    auto c_str = std::to_string(c);

    if (std::regex_match(c_str, text_pattern) )
    {
        return TOKENIZER_STATE::MAKE_TEXT;
    }
    
    if (std::regex_match(c_str, digit_pattern) )
    {
        return TOKENIZER_STATE::MAKE_NUMBER;
    }

    throw std::runtime_error("Error: Tokenizer doesnt support character:" + c_str);

    return TOKENIZER_STATE {};
}

Tokenizer::Tokenizer(std::string _code) : code(std::move(_code)), code_size(code.size())
{
    if( state_regex_map.empty() )
    {
        state_regex_map[TOKENIZER_STATE::MAKE_TEXT] = R"(^[a-zA-Z_]$)"; 
        state_regex_map[TOKENIZER_STATE::MAKE_NUMBER] = R"(^[0-9]$)"; 
    }
}



#endif