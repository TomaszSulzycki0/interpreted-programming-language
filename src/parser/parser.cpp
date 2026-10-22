#include<iostream>

#include"parser/parser.hpp"
#include"core/numericVariable.hpp"
#include"core/expression.hpp"
#include"debugMacros.hpp"

// Runs the lexer.
// Make sure to call before parseProgram()
void Parser::tokenizeProgram()
{
    tokens = tokenizer.emitTokens();
    tokens_size = tokens.size();

    #ifdef DEBUG_LEXER
        tokenizer.debugTokens(tokens);
    #endif
}

// Produces the AST.
// Call with PARSING_MODE::DEFAULT when calling for the first time
std::vector<std::unique_ptr<ASTNode>> Parser::parseProgram(const PARSING_MODE& mode) 
{
    std::vector<std::unique_ptr<ASTNode>> ast;

    while ( !isDone() )
    {
        try 
        {
            if ( check( TOKEN_TYPE::TOKEN_KEYWORD_TYPE ) )
            {
                ast.push_back( parseDeclaration() );
            }
            else if ( check( TOKEN_TYPE::TOKEN_KEYWORD_USER_DEFINED_TYPE) )
            {
                ast.push_back( parseObjectDeclaration() );
            }
            else if ( check( TOKEN_TYPE::TOKEN_KEYWORD_IF) )
            {
                ast.push_back( parseIf() );
            }
            else if ( check( TOKEN_TYPE::TOKEN_KEYWORD_WHILE) )
            {
                ast.push_back( parseWhile() );
            }
            else if ( check( TOKEN_TYPE::TOKEN_IDENTIFIER) )
            {
                ast.push_back( parseAssignment() );
            }
            else if ( check( TOKEN_TYPE::TOKEN_KEYWORD_FUNCTION ) )
            {
                ast.push_back( parseFunctionDeclaration() );
            }
            else if ( check( TOKEN_TYPE::TOKEN_KEYWORD_OBJECT ) )
            {
                ast.push_back( parseObjectSignature() );
            }
            else if ( check( TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER ) )
            {
                ast.push_back( parseVoidFunctionCall() );
            }
            else if ( check( TOKEN_TYPE::TOKEN_COMMENT_START ) )
            {
                advance();
                consume( TOKEN_TYPE::TOKEN_COMMENT_END, std::string_view("Error: Expected comment end token.") );
            }
            else if ( check( TOKEN_TYPE::TOKEN_RETURN ) )
            {
                ast.push_back( parseReturn() );
            }
            else if (   ( check( TOKEN_TYPE::TOKEN_EOF ) ) ||
                        ( check( TOKEN_TYPE::TOKEN_BRACE_CLOSE ) && ( mode == PARSING_MODE::FUNCTION_BODY || mode == PARSING_MODE::IF || mode == PARSING_MODE::WHILE ) ) 
                    )
            {
                break;
            }
            else
            {
                throw std::runtime_error("Error: Invalid statement starting syntax.");
            }
        }
        catch ( const std::exception& e )
        {
            is_good_for_exec = false;

            std::cerr << e.what() << " Line: " << peek().line << '\n';

            while ( !isDone() )
            {
                if (    check( TOKEN_TYPE::TOKEN_SEMICOLON ) || 
                        ( check( TOKEN_TYPE::TOKEN_BRACE_CLOSE ) && (mode == PARSING_MODE::FUNCTION_BODY || mode == PARSING_MODE::IF || mode == PARSING_MODE::WHILE ) ) )
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

bool Parser::isAtomicExpr(Token t) const
{
    return  t.type == TOKEN_TYPE::TOKEN_IDENTIFIER ||
            t.type == TOKEN_TYPE::TOKEN_LITERAL_FLOAT ||
            t.type == TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL ||
            t.type == TOKEN_TYPE::TOKEN_STRING_START ||
            t.type == TOKEN_TYPE::TOKEN_KEYWORD_BOOL;     
}

Token Parser::consume(TOKEN_TYPE type, std::string_view error_message) 
{
    if ( check(type) ) 
    { 
        return advance();
    }
    throw std::runtime_error( std::string(error_message) );
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    Token assignee_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected declaration identifier.") );
    
    std::unique_ptr<NamedExpression> left_side = std::make_unique<VariableExpression>( std::string(assignee_name.value) );
    
    while ( check(TOKEN_TYPE::TOKEN_DOT) )
    {
        advance();
        
        Token member_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected member identifier.") );
    
        left_side = std::make_unique<AccessMemberExpression>(
            std::move(left_side),
            std::string(member_name.value) );
    }
    
    // Check assignment operator
    Token assignment_operator = peek();

    std::unique_ptr<Expression> expr = nullptr;

    if (    assignment_operator.type == TOKEN_TYPE::TOKEN_PLUS_EQUALS ||
            assignment_operator.type == TOKEN_TYPE::TOKEN_MINUS_EQUALS ||
            assignment_operator.type == TOKEN_TYPE::TOKEN_MUL_EQUALS ||
            assignment_operator.type == TOKEN_TYPE::TOKEN_DIV_EQUALS )
    {
        // Eat compound assignment operator
        advance();

        expr = rpner.parseRPN(*this);

        // Change expression to include the left side of the assignment 
        expr = std::make_unique<BinaryExpression>( 
                std::string( 1, assignment_operator.value.front() ),
                left_side->clone(), 
                std::move( expr ) );
    }
    else
    {
        // If assignment operator not compound - has to be regular assignment
        consume( TOKEN_TYPE::TOKEN_EQUALS, std::string_view("Error: Expected equal sign.") );

        expr = rpner.parseRPN(*this);
    }

    if ( !expr )
    {
        throw std::runtime_error("Error: Could not parse assignment expression");
    }
    
    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<AssignmentNode>( std::move(left_side), std::move(expr) );
}

std::unique_ptr<ASTNode> Parser::parseDeclaration()
{
    Token declared_type = consume( TOKEN_TYPE::TOKEN_KEYWORD_TYPE, std::string_view("Error: Expected declaration type.") );

    Token declared_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Invalid declaration syntax.") );
    std::unique_ptr<Expression> expr = nullptr;
    
    // Assignment is optional
    // int x; is allowed - results in a default value
    // See NodeMaker::visit(const DeclarationNode& node)
    if ( check( TOKEN_TYPE::TOKEN_EQUALS ) )
    {
        advance();
        expr = rpner.parseRPN(*this);
        if ( !expr )
        {
            throw std::runtime_error("Error: Could not parse declaration expression");
        }
    }

    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<DeclarationNode>(   std::string( declared_type.value ) , 
                                                std::string( declared_name.value ), 
                                                std::move( expr ) );
}

std::unique_ptr<ASTNode> Parser::parseObjectDeclaration()
{
    Token declared_type = consume( TOKEN_TYPE::TOKEN_KEYWORD_USER_DEFINED_TYPE, std::string_view("Error: Expected object declaration type.") );

    Token declared_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Invalid declaration syntax.") );

    std::unique_ptr<MultiExpression> expression = nullptr;

    if ( check(TOKEN_TYPE::TOKEN_EQUALS) )
    {
        advance();

        if ( check(TOKEN_TYPE::TOKEN_BRACE_OPEN) )
        {
            expression = parseObjectInitialization();    
        }

    }

    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<ObjectDeclarationNode>( std::string(declared_type.value), std::string(declared_name.value), std::move(expression));
}

std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration()
{
     consume( TOKEN_TYPE::TOKEN_KEYWORD_FUNCTION, std::string_view("Error: Expected keyword fn.") );

    Token fn_identifier = consume( TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, std::string_view("Error: Expected function identifier.") );

    consume( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, std::string_view("Error: Expected opening parenthesis.") );

    std::vector<std::unique_ptr<DeclarableNode>> fn_args;

    if ( check( TOKEN_TYPE::TOKEN_KEYWORD_TYPE ) || check( TOKEN_TYPE::TOKEN_KEYWORD_USER_DEFINED_TYPE ) )
    {
        do
        {
            Token fn_arg_type = advance();
            Token fn_arg_identifier = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected function argument identifier.") );

            if ( fn_arg_type.type == TOKEN_TYPE:: TOKEN_KEYWORD_TYPE )
            {
                fn_args.push_back( std::make_unique<DeclarationNode>( std::string( fn_arg_type.value ), 
                std::string( fn_arg_identifier.value ), 
                nullptr ) );
            }
            else 
            {
                fn_args.push_back( std::make_unique<ObjectDeclarationNode>( std::string( fn_arg_type.value ), 
                std::string( fn_arg_identifier.value ), 
                nullptr ) );
            }

            if ( check( TOKEN_TYPE::TOKEN_COMMA ) )
            {
                advance();
            }
            else if( check( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE ) )
            {
                break;
            }
        }
        while ( !isDone() );
        
    }

    consume( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, std::string_view("Error: Unexpected keyword after opening parenthesis.") );
    
    consume( TOKEN_TYPE::TOKEN_ARROW, std::string_view("Error: Expected return type identification arrow '->'.") );

    Token fn_ret_type;
    if ( check( TOKEN_TYPE::TOKEN_KEYWORD_TYPE ) )
    {
        fn_ret_type = consume( TOKEN_TYPE::TOKEN_KEYWORD_TYPE, std::string_view("Error: Expected function return type.") );
    }
    else
    {
        fn_ret_type = consume( TOKEN_TYPE::TOKEN_KEYWORD_USER_DEFINED_TYPE, std::string_view("Error: Expected function return type.") );
    }

    consume( TOKEN_TYPE::TOKEN_BRACE_OPEN, std::string_view("Error: Expected opening bracket.") );

    auto fn_body = parseProgram( PARSING_MODE::FUNCTION_BODY );

    consume( TOKEN_TYPE::TOKEN_BRACE_CLOSE, std::string_view("Error: Expected closing bracket.") );

    return std::make_unique<FunctionDeclarationNode>(   std::string( fn_ret_type.value ),
                                                        std::string( fn_identifier.value ),
                                                        std::move( fn_args ),
                                                        std::move( fn_body ) );
}

std::unique_ptr<ASTNode> Parser::parseVoidFunctionCall()
{
    Token fn_identifier = consume( TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, std::string_view("Error: Expected function identifier.") );
    
    consume( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, std::string_view("Error: Expected opening parenthesis.") );

    auto fn_expr = parseFunctionCallArguments( fn_identifier.value );
    
    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<FunctionCallNode>( std::string( fn_identifier.value ), std::move( fn_expr) );
}

std::unique_ptr<ASTNode> Parser::parseIf()
{
    consume( TOKEN_TYPE::TOKEN_KEYWORD_IF, std::string_view("Error: Expected 'if' statement.") );
    consume( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, std::string_view("Error: Expected opening parenthesis after 'if' statement.") );

    auto if_condition = rpner.parseRPNCondition( *this );
    
    consume( TOKEN_TYPE::TOKEN_BRACE_OPEN, std::string_view("Error: Expected opening bracket.") );

    auto if_body = parseProgram( PARSING_MODE::IF );

    consume( TOKEN_TYPE::TOKEN_BRACE_CLOSE, std::string_view("Error: Expected closing bracket.") );

    std::unique_ptr<ElseNode> else_node_ptr = nullptr;

    if ( check( TOKEN_TYPE::TOKEN_KEYWORD_ELSE ) )
    {
        advance();

        consume( TOKEN_TYPE::TOKEN_BRACE_OPEN, std::string_view("Error: Expected opening bracket.") );

        auto else_body = parseProgram( PARSING_MODE::IF );

        consume( TOKEN_TYPE::TOKEN_BRACE_CLOSE, std::string_view("Error: Expected closing bracket.") );

        else_node_ptr = std::make_unique<ElseNode>( std::move( else_body ) );
    }

    return std::make_unique<IfNode>( std::move( if_body ), std::move( if_condition ), std::move( else_node_ptr ) );
}

std::unique_ptr<ASTNode> Parser::parseWhile()
{
    consume( TOKEN_TYPE::TOKEN_KEYWORD_WHILE, std::string_view("Error: Expected 'while' statement.") );
    consume( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, std::string_view("Error: Expected opening parenthesis after 'while' statement.") );

    auto while_condition = rpner.parseRPNCondition( *this );
    
    consume( TOKEN_TYPE::TOKEN_BRACE_OPEN, std::string_view("Error: Expected opening bracket.") );

    auto while_body = parseProgram( PARSING_MODE::IF );

    consume( TOKEN_TYPE::TOKEN_BRACE_CLOSE, std::string_view("Error: Expected closing bracket.") );

    return std::make_unique<WhileNode>( std::move( while_body ), std::move( while_condition ));
}

std::unique_ptr<ASTNode> Parser::parseReturn()
{
    consume( TOKEN_TYPE::TOKEN_RETURN, std::string_view("Error: Expected return statement.") );

    std::unique_ptr<Expression> expr = nullptr;
    
    expr = rpner.parseRPN(*this);

    // "return;" is allowed

    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<ReturnNode>( std::move(expr) );
}

std::unique_ptr<Expression> Parser::parseAtomicExpression()
{
    if ( check( TOKEN_TYPE::TOKEN_STRING_START ) )
    {
        // Token sequences allowed: 
        // TOKEN_STRING_START, TOKEN_STRING_END - empty string
        // TOKEN_STRING_START, TOKEN_STRING_BODY, TOKEN_STRING_END - regular string with body

        // Eat string start "
        advance(); 

        // String body or end if empty string
        Token str_body_or_end = advance(); 
        
        if ( str_body_or_end.type == TOKEN_TYPE::TOKEN_STRING_END )
        {
            return std::make_unique<LiteralExpression>("", "string");
        }
        else if ( str_body_or_end.type == TOKEN_TYPE::TOKEN_STRING_BODY )
        {
            consume( TOKEN_TYPE::TOKEN_STRING_END, std::string_view("Error: Expected string end literal.") );
            return std::make_unique<LiteralExpression>("\"" + std::string(str_body_or_end.value) + "\"", "string");
        }
        return nullptr;
    }

    if ( check( TOKEN_TYPE::TOKEN_LITERAL_FLOAT ) )
    {
        Token literal = advance();
        return std::make_unique<LiteralExpression>( std::string(literal.value), "float" );
    }

    if ( check( TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL ) )
    {
        Token literal = advance();
        return std::make_unique<LiteralExpression>( std::string(literal.value), "int" );
    }
    
    if ( check( TOKEN_TYPE::TOKEN_KEYWORD_BOOL ) )
    {
        Token literal = advance();
        return std::make_unique<LiteralExpression>( std::string(literal.value), "bool" );
    }
    
    if ( check( TOKEN_TYPE::TOKEN_IDENTIFIER ) )
    {
        Token identifier = advance();

        std::unique_ptr<NamedExpression> expr = std::make_unique<VariableExpression>( std::string(identifier.value) );

        // TODO: Make member access (dot) possible after funtion call eg. foo().x

        while ( check(TOKEN_TYPE::TOKEN_DOT ) )
        {
            advance();

            // TODO: Calling object methods

            Token member = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected identifier.") ); 

            expr = std::make_unique<AccessMemberExpression>( std::move(expr), std::string(member.value) );
        }

        return expr;
    }
    
    return nullptr;
} 

std::unique_ptr<FunctionCallExpression> Parser::parseFunctionCallArguments( std::string_view f_identifier )
{
    std::vector<std::unique_ptr<Expression>> args;
            
    bool end_of_args = false;

    while ( !end_of_args )
    {
        std::unique_ptr<Expression> arg_expr = rpner.parseFunctionCallRPN( *this, end_of_args );

        if ( arg_expr )
        {
            args.push_back( std::move( arg_expr ) );
        }
    }
    
    return std::make_unique<FunctionCallExpression>( std::string( f_identifier ), std::move( args ) );
} 

std::unique_ptr<FunctionCallExpression> Parser::parseNonVoidFunctionCall()
{
    Token fn_identifier = consume( TOKEN_TYPE::TOKEN_FUNCTION_IDENTIFIER, std::string_view("Error: Expected function identifier.") );

    consume( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, std::string_view("Error: Expected opening parenthesis.") );
    
    return parseFunctionCallArguments( fn_identifier.value );
}

std::unique_ptr<ASTNode> Parser::parseObjectSignature()
{
    consume(TOKEN_TYPE::TOKEN_KEYWORD_OBJECT, std::string_view("Error: Expected object keyword.") );
    
    Token identifier = consume(TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected valid object identifier.") );

    consume(TOKEN_TYPE::TOKEN_BRACE_OPEN, std::string_view("Error: Expected opening bracket.") );
    
    std::vector<std::unique_ptr<ASTNode>> member_vars {};

    while ( !check(TOKEN_TYPE::TOKEN_BRACE_CLOSE) && !isDone() )
    {
        if ( check(TOKEN_TYPE::TOKEN_KEYWORD_USER_DEFINED_TYPE) )
        {
            member_vars.push_back( parseObjectDeclaration() );
        }
        else if ( check(TOKEN_TYPE::TOKEN_KEYWORD_TYPE) )
        {
            member_vars.push_back( parseDeclaration() );
        }
        else
        {
            throw std::runtime_error("Error: Unexpected symbol in object member declaration.");
        }
    }

    consume(TOKEN_TYPE::TOKEN_BRACE_CLOSE, std::string_view("Error: Expected closing bracket.") );

    return std::make_unique<ObjectSignatureNode>( std::string(identifier.value), std::move(member_vars) );
}

std::unique_ptr<MultiExpression> Parser::parseObjectInitialization()
{
    consume(TOKEN_TYPE::TOKEN_BRACE_OPEN, std::string_view("Error: Expected opening bracket.") );

    std::vector<std::unique_ptr<Expression>> expressions;

    bool is_arg_end = false;

    while( !is_arg_end )
    {
        std::unique_ptr<Expression> expr = nullptr;
        expr = rpner.parseObjectRPN(*this, is_arg_end);
        if ( !expr )
        {
            throw std::runtime_error("Error: Could not parse object initialization argument expression");
        }

        expressions.push_back(std::move(expr));

        // Nested brackets
        if ( check(TOKEN_TYPE::TOKEN_BRACE_OPEN) && !is_arg_end )
        {
            expressions.push_back( parseObjectInitialization() );
        }

        if ( check(TOKEN_TYPE::TOKEN_BRACE_CLOSE) )
        {
            advance();
            is_arg_end = true;
        }
    }

    return std::make_unique<MultiExpression>(std::move(expressions));

}