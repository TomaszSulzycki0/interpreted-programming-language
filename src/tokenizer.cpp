#include"tokenizer.hpp"

Tokenizer::Tokenizer(const std::string& _code) : code(_code), code_size(code.size())
{
    
}

std::vector<Token> Tokenizer::emitTokens()
{
    std::vector<Token> tokens {};

    while( pos < code_size )
    {
        tokens.push_back( getNextToken() );    
    }

    //tokens.push_back( Token{ TOKEN_TYPE::TOKEN_EOF, std::string_view("EOF") } ); 

    return tokens;
}

Token Tokenizer::getNextToken() 
{
    switch ( currentState() ) 
    {
        case TOKENIZER_STATE::DEFAULT:  return useStateDefault();
        case TOKENIZER_STATE::STRING:   return useStateString();
        case TOKENIZER_STATE::COMMENT:  return useStateComment();
    }
    
    std::unreachable();
}

Token Tokenizer::readIdentifier()
{
    const std::size_t start_pos = pos;

    advance(); // Already read identifier start before function call

    while ( pos < code_size && isIdentifierBody( peek() ) ) 
    {
        pos++;
    }

    const std::string_view lexeme = code.substr(start_pos, pos - start_pos);

    auto it = keywords.find(lexeme);
    if ( it != keywords.end() )
    {
        return Token{ it->second, lexeme };
    }
    
    return Token{ TOKEN_TYPE::TOKEN_IDENTIFIER, lexeme };
}

Token Tokenizer::readNumber()
{
    const std::size_t start_pos = pos;
    bool is_float = false;

    advance();

    while ( pos < code_size && isDigit( peek() ) ) 
    {
        advance();
    }

    if ( peek() == '.' && isDigit( peekNext() ) ) 
    {
        is_float = true;

        // dot followed by digit - we can skip two
        advance();
        advance();
    
        while ( pos < code_size && isDigit( peek() ) ) 
        {
            advance();
        }
    }

    const std::string_view lexeme = code.substr(start_pos, pos - start_pos);

    if (is_float) 
    {
        return Token{ TOKEN_TYPE::TOKEN_LITERAL_FLOAT, lexeme };
    } 
    
    return Token{ TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL, lexeme };    
}

Token Tokenizer::useStateDefault()
{
    skipWhitespace();

    if ( pos >= code_size ) 
    {
        return Token{ TOKEN_TYPE::TOKEN_EOF };
    }

    char c = peek(); 
    
    if ( isIdentifierStart(c) )
    {
        return readIdentifier(); 
    } 
    else if ( isDigit(c) )
    {
        return readNumber(); 
    }

    advance();

    switch (c)
    {
    case '"':
        pushState(TOKENIZER_STATE::STRING);
        return Token{ TOKEN_TYPE::TOKEN_STRING_START,  std::string_view( "\"") };
    case '#':
        pushState(TOKENIZER_STATE::COMMENT);
        return Token{ TOKEN_TYPE::TOKEN_COMMENT_START, std::string_view("#") };
    case ';':
        return Token{ TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view(";") };
    case '+':
        return Token{ TOKEN_TYPE::TOKEN_OPERATOR_PLUS, std::string_view("+") };
    case '-':
        skipWhitespace();
        if ( isDigit( peek() ) )
        {
            return Token{ TOKEN_TYPE::TOKEN_MINUS_SIGN, std::string_view("-") };
        }
        return Token{ TOKEN_TYPE::TOKEN_OPERATOR_MINUS, std::string_view("-") };
    case '*':
        return Token{ TOKEN_TYPE::TOKEN_OPERATOR_MUL, std::string_view("*") };
    case '/':
        return Token{ TOKEN_TYPE::TOKEN_OPERATOR_DIV, std::string_view("/") };
    case '=':
        return Token{ TOKEN_TYPE::TOKEN_OPERATOR_EQUALS, std::string_view("=") };
    case '(':
        return Token{ TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, std::string_view("(") };
    case ')':
        return Token{ TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, std::string_view(")") };
    }
    
    return Token{ TOKEN_TYPE::TOKEN_ERROR, std::string_view("Bad character") };
}

Token Tokenizer::useStateString()
{
    if ( pos >= code_size ) 
    {
        popState();
        return Token{ TOKEN_TYPE::TOKEN_ERROR, std::string_view("Unterminated string literal") };
    }

    char c = peek();

    if ( c == '"' )
    {
        advance(); 
        popState(); 
        return Token{ TOKEN_TYPE::TOKEN_STRING_END, std::string_view("\"") };
    }
    else if ( c == '\n' ) 
    {
        advance();
        popState(); 
        return Token{ TOKEN_TYPE::TOKEN_ERROR, std::string_view("Forbidden newline in string") };
    }

    size_t start_in_buffer = str_buffer.size();

    while ( pos < code_size && peek() != '"' && peek() != '\n' ) 
    {
        if ( peek() == '\\')
        {
            advance();
            if ( pos  >= code_size )
            {
                break;
            }
            char after_escape = advance();
            char escape_decoded = decodeEscapeSeq( after_escape );
            str_buffer.push_back( escape_decoded );
            continue;
        }
        str_buffer.push_back( advance() );
    }

    const std::string_view lexeme( &str_buffer[start_in_buffer], str_buffer.size() - start_in_buffer );

    return Token{ TOKEN_TYPE::TOKEN_STRING_BODY, lexeme };
}

Token Tokenizer::useStateComment()
{
    while ( pos < code_size && peek() != '\n' )
    {
        advance();
    }

    popState();
    return Token{ TOKEN_TYPE::TOKEN_COMMENT_END, std::string_view("#") };
}

bool Tokenizer::isIdentifierStart(char c) const
{
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Tokenizer::isIdentifierBody(char c) const
{
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

bool Tokenizer::isDigit(char c) const
{
    return std::isdigit(static_cast<unsigned char>(c));
}

void Tokenizer::skipWhitespace()
{
    char c = peek();   
    while ( std::isspace( static_cast<unsigned char>(c) ) )
    {
        advance();
        c = peek();
    }
}

char Tokenizer::decodeEscapeSeq(char c) const
{
    switch (c)
    {
        case 'n':   return '\n';
        case 't':   return '\t';
        case 'r':   return '\r';
        case '\\':  return '\\';
        case '\"':   return '\"';
        default: return '\\';
    }
}

std::ostream& operator<<(std::ostream& os, TOKEN_TYPE type)
{
    switch (type) 
    {
        case TOKEN_TYPE::TOKEN_IDENTIFIER:              return os << "IDENTIFIER";
        case TOKEN_TYPE::TOKEN_KEYWORD_TYPE:            return os << "TYPE";
        case TOKEN_TYPE::TOKEN_MINUS_SIGN:              return os << "MINUS_SIGN";
        case TOKEN_TYPE::TOKEN_OPERATOR_EQUALS:         return os << "OPERATOR_EQUALS";
        case TOKEN_TYPE::TOKEN_OPERATOR_DIV:            return os << "OPERATOR_DIV";
        case TOKEN_TYPE::TOKEN_OPERATOR_MUL:            return os << "OPERATOR_MUL";
        case TOKEN_TYPE::TOKEN_OPERATOR_PLUS:           return os << "OPERATOR_PLUS";
        case TOKEN_TYPE::TOKEN_OPERATOR_MINUS:          return os << "OPERATOR_MINUS";
        case TOKEN_TYPE::TOKEN_SEMICOLON:               return os << "SEMICOLON";
        case TOKEN_TYPE::TOKEN_LITERAL_FLOAT:           return os << "LITERAL_FLOAT";
        case TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL:        return os << "LITERAL_INTEGRAL";
        case TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN:        return os << "PARENTHESIS_OPEN";
        case TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE:       return os << "PARENTHESIS_CLOSE";
        case TOKEN_TYPE::TOKEN_STRING_START:            return os << "STRING_START";
        case TOKEN_TYPE::TOKEN_STRING_BODY:             return os << "STRING_BODY";
        case TOKEN_TYPE::TOKEN_STRING_END:              return os << "STRING_END";
        case TOKEN_TYPE::TOKEN_COMMENT_START:           return os << "COMMENT_START";
        case TOKEN_TYPE::TOKEN_COMMENT_END:             return os << "COMMENT_END";
        case TOKEN_TYPE::TOKEN_ERROR:                   return os << "ERROR";
        case TOKEN_TYPE::TOKEN_EOF:                     return os << "EOF";
        default:                                        return os << "UNKNOWN";
    }
}

void Tokenizer::debugTokens(const std::vector<Token>& tokens)
{
    for ( const auto& token : tokens )
    {
        std::cout   << std::left
                    << std::setw(0) << "[" 
                    << std::setw(17) << token.type 
                    << std::setw(0) << "]" 
                    << std::setw(7) << "  ->"
                    << std::setw(12) << token.value 
                    << std::endl;
    }
}