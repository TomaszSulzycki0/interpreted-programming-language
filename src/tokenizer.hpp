#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include<string>
#include<vector>
#include<memory>
#include<stdexcept>
#include<unordered_map>
#include<utility>
#include<iostream>
#include<iomanip>
#include<deque>


enum class TOKENIZER_STATE
{
    DEFAULT,      
    STRING,       
    COMMENT      
};

enum class TOKEN_TYPE
{
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD_TYPE,
    TOKEN_KEYWORD_BOOL,
    TOKEN_KEYWORD_FUNCTION,
    TOKEN_KEYWORD_IF,
    TOKEN_EQUALS,
    TOKEN_PLUS_EQUALS,
    TOKEN_MINUS_EQUALS,
    TOKEN_MUL_EQUALS,
    TOKEN_DIV_EQUALS,
    TOKEN_OPERATOR,
    TOKEN_UNARY_OPERATOR,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_ARROW,
    TOKEN_LITERAL_FLOAT,
    TOKEN_LITERAL_INTEGRAL,
    TOKEN_PARENTHESIS_OPEN,
    TOKEN_PARENTHESIS_CLOSE,
    TOKEN_BRACE_OPEN,
    TOKEN_BRACE_CLOSE,
    TOKEN_STRING_START,
    TOKEN_STRING_BODY,
    TOKEN_STRING_END,
    TOKEN_COMMENT_START,
    TOKEN_COMMENT_END,
    TOKEN_RETURN,
    TOKEN_ERROR,
    TOKEN_NULL,
    TOKEN_EOF
};

std::ostream& operator<<(std::ostream& os, TOKEN_TYPE type); 

struct Token
{
    TOKEN_TYPE type;
    std::string_view value {};
    std::size_t line {};
};

class Tokenizer
{
private:
    const static inline std::unordered_map<std::string_view, TOKEN_TYPE> keywords 
    {
        { std::string_view("int"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("float"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("double"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("bool"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("string"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("fn"), TOKEN_TYPE::TOKEN_KEYWORD_FUNCTION },
        { std::string_view("true"), TOKEN_TYPE::TOKEN_KEYWORD_BOOL },
        { std::string_view("false"), TOKEN_TYPE::TOKEN_KEYWORD_BOOL },
        { std::string_view("if"), TOKEN_TYPE::TOKEN_KEYWORD_IF },
        { std::string_view("and"), TOKEN_TYPE::TOKEN_OPERATOR },
        { std::string_view("or"), TOKEN_TYPE::TOKEN_OPERATOR },
        { std::string_view("return"), TOKEN_TYPE::TOKEN_RETURN }
    };

    std::vector<TOKENIZER_STATE> state_stack { TOKENIZER_STATE::DEFAULT };
    std::deque<char> str_buffer {};

    std::size_t pos {};
    std::size_t current_line {1};
    std::string_view code;
    const std::size_t code_size;

    TOKENIZER_STATE currentState() const { return state_stack.back(); }
    void pushState(TOKENIZER_STATE state) { state_stack.push_back(state); }
    void popState() { if ( state_stack.size() > 1 ) state_stack.pop_back(); }
    char advance() { return code[pos++]; }
    char peek() const { return pos < code_size ? code[pos] : '\0'; }
    char peekNext() const { return pos + 1 < code_size ? code[pos + 1] : '\0'; }
    char decodeEscapeSeq(char c) const;
    
    bool isIdentifierStart(char c) const;
    bool isIdentifierBody(char c) const;
    bool isDigit(char c) const;
    void skipWhitespace();
    void skipNewline();
    
    Token getNextToken(); 
    Token readIdentifier();
    Token readNumber();

    Token useStateDefault();
    Token useStateString();
    Token useStateComment();


public:
    explicit Tokenizer(const std::string& _code);
    std::vector<Token> emitTokens();
    void debugTokens(const std::vector<Token>& tokens);
};


#endif