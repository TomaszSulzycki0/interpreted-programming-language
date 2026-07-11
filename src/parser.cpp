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
    std::vector<std::unique_ptr<ASTNode>> ast;

    while ( pos < tokens_size )
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
    }

    return ast;
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    Token assignee_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected declaration identifier.") );
    
    consume( TOKEN_TYPE::TOKEN_OPERATOR_EQUALS, std::string_view("Error: Expected equal sign.") );
    
    std::unique_ptr<Expression> expr = parseExpression();

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
    Token declared_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected declaration identifier.") );
    std::unique_ptr<Expression> expr = nullptr;
    
    if ( check( TOKEN_TYPE::TOKEN_OPERATOR_EQUALS ) )
    {
        advance();
        expr = parseExpression();
        if ( !expr )
        {
            throw std::runtime_error("Error: Could not parse expression");
        }
    }

    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<DeclarationNode>( std::string(declared_type.value), std::string(declared_name.value), std::move(expr) );
}

std::unique_ptr<Expression> Parser::parseExpression()
{
    if ( check( TOKEN_TYPE::TOKEN_STRING_START ) )
    {
        advance();
        Token str_body = advance();
        
        if ( str_body.type == TOKEN_TYPE::TOKEN_STRING_BODY )
        {
            consume( TOKEN_TYPE::TOKEN_STRING_END, std::string_view("Error: Expected string end literal.") );
        }
        return std::make_unique<LiteralExpression>("\"" + std::string(str_body.value) + "\"");
    }

    if ( check( TOKEN_TYPE::TOKEN_OPERATOR_MINUS ) )
    {
        Token minus_sign = advance();
        if ( check( TOKEN_TYPE::TOKEN_LITERAL_FLOAT ) || check( TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL )) // Later can pass exact info to expression obj
        {
            Token literal = advance();
            return std::make_unique<LiteralExpression>( std::string(minus_sign.value) + std::string(literal.value) );
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