#include<iostream>

#include"parser.hpp"
#include"numericVariable.hpp"
#include"expression.hpp"

void AssignmentNode::accept(NodeVisitor& visitor) const 
{
    visitor.visit(*this); 
}

void DeclarationNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this); 
}

void Parser::tokenizeProgram()
{
    tokens = tokenizer.emitTokens();
    tokens_size = tokens.size();
    tokenizer.debugTokens(tokens);
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseProgram() 
{
    if ( tokens_size == 0 )
    {
        tokenizeProgram();
    }

    std::vector<std::unique_ptr<ASTNode>> ast;

    while ( pos < tokens_size )
    {
        try 
        {
            if ( check( TOKEN_TYPE::TOKEN_KEYWORD_TYPE ) )
            {
                ast.push_back( std::move( parseDeclaration() ) );
            }
            else if ( check( TOKEN_TYPE::TOKEN_IDENTIFIER) )
            {
                ast.push_back( std::move( parseAssignment() ) );
            }
            else if ( check( TOKEN_TYPE::TOKEN_COMMENT_START ) )
            {
                advance();
                consume( TOKEN_TYPE::TOKEN_COMMENT_END, std::string_view("Error: Expected comment end token.") );
            }
            else if ( check( TOKEN_TYPE::TOKEN_EOF ) )
            {
                break;
            }
            else
            {
                throw std::runtime_error("Error: Unrecognised token.");
            }
        }
        catch ( const std::exception& e )
        {
            is_good_for_exec = false;

            std::cerr << e.what() << " Line: " << peek().line << '\n';

            // For now simply skip to semicolon.
            while ( pos < tokens_size )
            {
                if ( check( TOKEN_TYPE::TOKEN_SEMICOLON ) )
                {
                    advance();
                    break;
                }
                advance();
            }
        }
    }

    return ast;
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    Token assignee_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected declaration identifier.") );
    
    
    consume( TOKEN_TYPE::TOKEN_OPERATOR_EQUALS, std::string_view("Error: Expected equal sign.") );
    
    std::unique_ptr<Expression> expr = parseRPN();

    if ( !expr )
    {
        throw std::runtime_error("Error: Could not parse expression");
    }
    
    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<AssignmentNode>( std::string(assignee_name.value), std::move(expr) );
}

std::unique_ptr<ASTNode> Parser::parseDeclaration()
{
    Token declared_type = consume( TOKEN_TYPE::TOKEN_KEYWORD_TYPE, std::string_view("Error: Expected declaration type.") );

    // More precise, for most common invalid syntax
    if ( check( TOKEN_TYPE::TOKEN_KEYWORD_TYPE ) )
    {
        consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Invalid combination type-type during declaration.") );
    }

    Token declared_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Invalid declaration syntax.") );
    std::unique_ptr<Expression> expr = nullptr;
    
    if ( check( TOKEN_TYPE::TOKEN_OPERATOR_EQUALS ) )
    {
        advance();
        expr = parseRPN();
        if ( !expr )
        {
            throw std::runtime_error("Error: Could not parse expression");
        }
    }

    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<DeclarationNode>( std::string(declared_type.value), std::string(declared_name.value), std::move(expr) );
}

std::unique_ptr<Expression> Parser::parseAtomicExpression()
{
    if ( check( TOKEN_TYPE::TOKEN_STRING_START ) )
    {
        advance(); // eat string start "
        Token str_body_or_end = advance(); // string body or end if empty string
        
        if ( str_body_or_end.type == TOKEN_TYPE::TOKEN_STRING_END ) // empty string case
        {
            return std::make_unique<LiteralExpression>("");
        }
        else if ( str_body_or_end.type == TOKEN_TYPE::TOKEN_STRING_BODY )
        {
            consume( TOKEN_TYPE::TOKEN_STRING_END, std::string_view("Error: Expected string end literal.") );
            return std::make_unique<LiteralExpression>("\"" + std::string(str_body_or_end.value) + "\"");
        }
        return nullptr;
    }

    if ( check( TOKEN_TYPE::TOKEN_MINUS_SIGN ) )
    {
        Token minus_sign = advance();
        if ( check( TOKEN_TYPE::TOKEN_LITERAL_FLOAT ) || check( TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL )) // Later can pass exact info to expression obj
        {
            Token literal = advance();
            return std::make_unique<LiteralExpression>( "-" + std::string(literal.value) );
        }
    }
    
    if ( check( TOKEN_TYPE::TOKEN_LITERAL_FLOAT ) || check( TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL )) // Later can pass exact info to expression obj
    {
        Token literal = advance();
        return std::make_unique<LiteralExpression>( std::string(literal.value) );
    }
    
    if ( check( TOKEN_TYPE::TOKEN_IDENTIFIER ) )
    {
        Token identifier = advance();
        return std::make_unique<VariableExpression>( std::string(identifier.value) );
    }
    
    return nullptr;
} 

std::unique_ptr<Expression> Parser::parseRPN()
{   
    const static std::unordered_map<TOKEN_TYPE, int> operator_precedence {
        { TOKEN_TYPE::TOKEN_OPERATOR_MINUS, 1 }, 
        { TOKEN_TYPE::TOKEN_OPERATOR_PLUS, 1 },
        { TOKEN_TYPE::TOKEN_OPERATOR_MUL, 2 },
        { TOKEN_TYPE::TOKEN_OPERATOR_DIV, 2 }
    };

    std::vector<Token> operator_stack {};
    std::vector<std::unique_ptr<Expression>> expr_stack {};
    int open_parenthesis = 0;

    while ( pos < tokens_size )
    {
        if ( isOperator( peek() ) )
        {
            Token op = advance();

            auto it = operator_precedence.find(op.type);
            if ( it == operator_precedence.end() )
            {
                throw std::runtime_error("Error: Unknown operator.");    
            }
        
            int op_precedence = it->second;
            
            while ( !operator_stack.empty() )
            {   
                auto last_op = operator_stack.back();
                auto _it = operator_precedence.find(last_op.type);

                if ( _it == operator_precedence.end() )
                {
                    // Next token on stack is not arithmetic operator (opening parenthesis)
                    break;
                }
                
                int last_op_precedence = _it->second;

                if ( last_op_precedence >= op_precedence )
                {
                    makeBinExprRPN( operator_stack, expr_stack );
                }
                else
                {
                    break;
                }
            }

            operator_stack.push_back( op );
        }
        else if ( check( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN ) )
        {
            operator_stack.push_back( advance() );
            ++open_parenthesis;
        }
        else if ( check( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE ) )
        {
            if ( open_parenthesis < 1 )
            {
                throw std::runtime_error("Error: Unexpected closing parenthesis without opening counterpart.");
            }

            advance();

            while ( !operator_stack.empty() )
            {   
                auto last_op = operator_stack.back();
                operator_stack.pop_back();
                
                if ( last_op.type == TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN )
                {
                    break;
                }

                if ( expr_stack.empty() )
                {
                    // ( ) - fine
                    break;
                }
                auto expr_right = std::move( expr_stack.back() );
                expr_stack.pop_back();

                if ( expr_stack.empty() )
                {
                    // ( myVar ) - fine. Leave it on the stack
                    expr_stack.push_back( std::move( expr_right) );
                    break;
                }
                auto expr_left = std::move( expr_stack.back() );
                expr_stack.pop_back();

                expr_stack.push_back( std::make_unique<BinaryExpression>( std::string(last_op.value), std::move( expr_left ), std::move( expr_right ) ) );
            }

            --open_parenthesis;

        }
        else if ( isAtomicExpr( peek() ) )
        {
            auto expr = parseAtomicExpression();
            if ( !expr )
            {
                throw std::runtime_error("Error: Could not parse expression");
            }
            expr_stack.push_back( std::move( expr ) );
        }
        else if ( check( TOKEN_TYPE::TOKEN_SEMICOLON ) )
        {
            break;
        }
        else
        {
            throw std::runtime_error("Error: Invalid expression");
        }
    }

    if ( open_parenthesis != 0 )
    {
        throw std::runtime_error("Error: Unclosed parenthesis.");
    }

    while ( !operator_stack.empty() )
    {
        makeBinExprRPN( operator_stack, expr_stack );   
    }

    if ( expr_stack.size() != 1 )
    {
        throw std::runtime_error("Error: Invalid expression");
    }

    return std::move( expr_stack.front() );
}

void Parser::makeBinExprRPN( std::vector<Token>& operator_stack, std::vector<std::unique_ptr<Expression>>& expr_stack)
{
    auto last_op = operator_stack.back();
    operator_stack.pop_back();

    if ( expr_stack.empty() )
    {
        throw std::runtime_error("Error: Unexpected operator.");
    }

    auto expr_right = std::move( expr_stack.back() );
    expr_stack.pop_back();

    if ( expr_stack.empty() )
    {
        throw std::runtime_error("Error: Unexpected operator.");
    }
    auto expr_left = std::move( expr_stack.back() );
    expr_stack.pop_back();

    expr_stack.push_back( std::make_unique<BinaryExpression>( 
            std::string(last_op.value), 
            std::move( expr_left ), 
            std::move( expr_right ) ) );
}