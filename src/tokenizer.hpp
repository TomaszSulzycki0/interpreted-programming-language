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
    TOKEN_OPERATOR_EQUALS,
    TOKEN_OPERATOR_PLUS,
    TOKEN_OPERATOR_MINUS,
    TOKEN_OPERATOR_DIV,
    TOKEN_OPERATOR_MUL,
    TOKEN_MINUS_SIGN,
    TOKEN_SEMICOLON,
    TOKEN_LITERAL_FLOAT,
    TOKEN_LITERAL_INTEGRAL,
    TOKEN_PARENTHESIS_OPEN,
    TOKEN_PARENTHESIS_CLOSE,
    TOKEN_STRING_START,
    TOKEN_STRING_BODY,
    TOKEN_STRING_END,
    TOKEN_COMMENT_START,
    TOKEN_COMMENT_END,
    TOKEN_KEYWORD_BOOL,
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
        { std::string_view("i"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("f"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("d"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("b"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("s"), TOKEN_TYPE::TOKEN_KEYWORD_TYPE },
        { std::string_view("true"), TOKEN_TYPE::TOKEN_KEYWORD_BOOL },
        { std::string_view("false"), TOKEN_TYPE::TOKEN_KEYWORD_BOOL }
    };

    std::vector<TOKENIZER_STATE> state_stack { TOKENIZER_STATE::DEFAULT };
    std::deque<char> str_buffer {};

    std::size_t pos {};
    std::size_t current_line {1};
    std::string_view code;
    const std::size_t code_size;

    Token last_emmited = { TOKEN_TYPE::TOKEN_NULL };

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