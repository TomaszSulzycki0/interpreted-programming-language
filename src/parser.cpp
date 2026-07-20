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

void FunctionDeclarationNode::accept(NodeVisitor& visitor) const
{
    visitor.visit(*this); 
}

void FunctionCallNode::accept(NodeVisitor& visitor) const
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
        // In case the tokenizer didnt run
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
            else if ( check( TOKEN_TYPE::TOKEN_KEYWORD_FUNCTION ) )
            {
                ast.push_back( std::move( parseFunctionDeclaration() ) );
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
                throw std::runtime_error("Error: Invalid statement starting syntax.");
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

std::vector<std::unique_ptr<ASTNode>> Parser::parseFunctionBody()
{
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
            else if ( check( TOKEN_TYPE::TOKEN_KEYWORD_FUNCTION ) )
            {
                ast.push_back( std::move( parseFunctionDeclaration() ) );
            }
            else if ( check( TOKEN_TYPE::TOKEN_COMMENT_START ) )
            {
                advance();
                consume( TOKEN_TYPE::TOKEN_COMMENT_END, std::string_view("Error: Expected comment end token.") );
            }
            else if (   check( TOKEN_TYPE::TOKEN_RETURN ) || 
                        check( TOKEN_TYPE::TOKEN_EOF ) ||
                        check( TOKEN_TYPE::TOKEN_BRACE_CLOSE) )
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

            // For now simply skip to semicolon.
            while ( pos < tokens_size )
            {
                if ( check( TOKEN_TYPE::TOKEN_SEMICOLON ) || check( TOKEN_TYPE::TOKEN_BRACE_CLOSE ) )
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

        expr = parseRPN();

        // Change expression to include the left side of the assignment 
        auto left_side = std::make_unique<VariableExpression>( std::string( assignee_name.value ) );
        expr = std::make_unique<BinaryExpression>( 
                std::string( 1, assignment_operator.value.front() ),
                std::move( left_side ), 
                std::move( expr ) );
    }
    else
    {
        // If assignment operator not compound - has to be regular assignment
        consume( TOKEN_TYPE::TOKEN_EQUALS, std::string_view("Error: Expected equal sign.") );

        expr = parseRPN();
    }

    if ( !expr )
    {
        throw std::runtime_error("Error: Could not parse assignment expression");
    }
    
    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<AssignmentNode>( std::string(assignee_name.value), std::move(expr) );
}

std::unique_ptr<ASTNode> Parser::parseDeclaration()
{
    Token declared_type = consume( TOKEN_TYPE::TOKEN_KEYWORD_TYPE, std::string_view("Error: Expected declaration type.") );

    // More precise, for common invalid syntax int int = 1;
    if ( check( TOKEN_TYPE::TOKEN_KEYWORD_TYPE ) )
    {
        consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Invalid combination type-type during declaration.") );
    }

    Token declared_name = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Invalid declaration syntax.") );
    std::unique_ptr<Expression> expr = nullptr;
    
    // Assignment is optional
    // int x; is allowed - results in a default value
    // See NodeMaker::visit(const DeclarationNode& node)
    if ( check( TOKEN_TYPE::TOKEN_EQUALS ) )
    {
        advance();
        expr = parseRPN();
        if ( !expr )
        {
            throw std::runtime_error("Error: Could not parse declaration expression");
        }
    }

    consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );

    return std::make_unique<DeclarationNode>( std::string(declared_type.value), std::string(declared_name.value), std::move(expr) );
}

std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration()
{
    consume( TOKEN_TYPE::TOKEN_KEYWORD_FUNCTION, std::string_view("Error: Expected keyword fn.") );

    Token fn_identifier = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected function identifier.") );

    consume( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN, std::string_view("Error: Expected opening parenthesis.") );

    std::vector<std::unique_ptr<DeclarationNode>> fn_args;

    if ( check( TOKEN_TYPE::TOKEN_KEYWORD_TYPE ) )
    {
        do
        {
            Token fn_arg_type = advance();
            Token fn_arg_identifier = consume( TOKEN_TYPE::TOKEN_IDENTIFIER, std::string_view("Error: Expected function argument identifier.") );
            fn_args.push_back( std::make_unique<DeclarationNode>( std::string(fn_arg_type.value), std::string(fn_arg_identifier.value), nullptr ) );

            if ( check( TOKEN_TYPE::TOKEN_COMMA ) )
            {
                advance();
            }
            else if( check( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE ) )
            {
                break;
            }
        }
        while ( pos < tokens_size );
        
    }

    consume( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, std::string_view("Error: Expected closing parenthesis.") );
    
    consume( TOKEN_TYPE::TOKEN_ARROW, std::string_view("Error: Expected return type identification arrow '->'.") );

    Token fn_ret_type = consume( TOKEN_TYPE::TOKEN_KEYWORD_TYPE, std::string_view("Error: Expected function return type.") );

    consume( TOKEN_TYPE::TOKEN_BRACE_OPEN, std::string_view("Error: Expected opening bracket.") );

    auto fn_body = parseFunctionBody();

    std::unique_ptr<Expression> fn_ret_expr = nullptr;

    if ( check( TOKEN_TYPE::TOKEN_RETURN ) )
    {
        advance();
        fn_ret_expr = parseRPN();
        consume( TOKEN_TYPE::TOKEN_SEMICOLON, std::string_view("Error: Expected semicolon.") );
    }

    consume( TOKEN_TYPE::TOKEN_BRACE_CLOSE, std::string_view("Error: Expected closing bracket.") );

    return std::make_unique<FunctionDeclarationNode>(   std::string( fn_ret_type.value ),
                                                        std::string( fn_identifier.value ),
                                                        std::move( fn_args ),
                                                        std::move( fn_body ),
                                                        std::move( fn_ret_expr ) );
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
            return std::make_unique<LiteralExpression>("");
        }
        else if ( str_body_or_end.type == TOKEN_TYPE::TOKEN_STRING_BODY )
        {
            consume( TOKEN_TYPE::TOKEN_STRING_END, std::string_view("Error: Expected string end literal.") );
            return std::make_unique<LiteralExpression>("\"" + std::string(str_body_or_end.value) + "\"");
        }
        return nullptr;
    }
    
    if (    check( TOKEN_TYPE::TOKEN_LITERAL_FLOAT ) || 
            check( TOKEN_TYPE::TOKEN_LITERAL_INTEGRAL ) || 
            check( TOKEN_TYPE::TOKEN_KEYWORD_BOOL ) ) // Later can pass exact info to expression obj
    {
        Token literal = advance();
        return std::make_unique<LiteralExpression>( std::string(literal.value) );
    }
    
    if ( check( TOKEN_TYPE::TOKEN_IDENTIFIER ) )
    {
        Token identifier = advance();

        if ( check( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN ) )
        {
            advance();
            std::vector<std::unique_ptr<Expression>> args;
            while( !check( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE ) && pos < tokens_size)
            {
                std::unique_ptr<Expression> arg_expr = parseAtomicExpression();

                if( !arg_expr )
                {
                    throw std::runtime_error("Error: Could not parse function argument");
                }

                args.push_back( std::move( arg_expr ) );

                if ( check( TOKEN_TYPE::TOKEN_COMMA ) )
                {
                    advance();
                    // fix: "foo(x,)" is valid - should not be
                }
            }

        consume( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE, std::string_view("Error: Expected closing parenthesis.") );

            return std::make_unique<FunctionCallExpression>( std::string(identifier.value), std::move( args ) ); 
        }

        return std::make_unique<VariableExpression>( std::string(identifier.value) );
    }
    
    return nullptr;
} 

std::unique_ptr<Expression> Parser::parseRPN()
{   
    const static std::unordered_map<std::string, int> operator_precedence {
        { "or", 0 }, 
        { "and", 0 }, 
        { "<", 0 }, 
        { ">", 0 }, 
        { "==", 0 }, 
        { ">=", 0 }, 
        { "<=", 0 }, 
        { "-", 1 }, 
        { "+", 1 },
        { "*", 2 },
        { "/", 2 }
    };
    
    const int unary_precedence = 3;

    std::vector<Token> operator_stack {};
    std::vector<std::unique_ptr<Expression>> expr_stack {};

    // Counter for validating that ever open parenthesis is closed
    int open_parenthesis = 0;

    // Flag used for identifying unary operators
    // Set to true if next token is expected to be a value, false otherwise
    // ...) - x -> minus is not unary
    // ...( - x -> minus is unary
    bool expect_value = true;

    while ( pos < tokens_size )
    {
        if ( peek().type == TOKEN_TYPE::TOKEN_OPERATOR && expect_value )
        {
            // There should be a procedure for handling the operator stack here
            // For now assume that unary operators have the highest possible precedence
            Token op = advance();
            op.type = TOKEN_TYPE::TOKEN_UNARY_OPERATOR;

            operator_stack.push_back( op );
        }
        else if ( peek().type == TOKEN_TYPE::TOKEN_OPERATOR )
        {
            Token op = advance();

            auto it = operator_precedence.find( std::string(op.value) );
            if ( it == operator_precedence.end() )
            {
                throw std::runtime_error("Error: Unknown operator.");    
            }
            int op_precedence = it->second;
            
            while ( !operator_stack.empty() )
            {   
                // Main RPN logic

                auto last_op = operator_stack.back();
                auto _it = operator_precedence.find( std::string(last_op.value) );

                if ( _it == operator_precedence.end() )
                {
                    // Next token on stack is not arithmetic operator (opening parenthesis)
                    break;
                }
                
                int last_op_precedence = _it->second;
                
                if ( last_op.type == TOKEN_TYPE::TOKEN_UNARY_OPERATOR )
                {
                    last_op_precedence = unary_precedence;
                }

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
            expect_value = true;
        }
        else if ( check( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN ) )
        {
            operator_stack.push_back( advance() );
            expect_value = true;
            ++open_parenthesis;
        }
        else if ( check( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE ) )
        {
            if ( open_parenthesis < 1 )
            {
                throw std::runtime_error("Error: Unexpected closing parenthesis without opening counterpart.");
            }

            // Eat parenthesis
            advance();

            while ( !operator_stack.empty() )
            {   
                // RPN logic

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
                    // ( x ) - fine. Leave it on the stack
                    expr_stack.push_back( std::move( expr_right) );
                    break;
                }
                auto expr_left = std::move( expr_stack.back() );
                expr_stack.pop_back();

                expr_stack.push_back( std::make_unique<BinaryExpression>( std::string(last_op.value), std::move( expr_left ), std::move( expr_right ) ) );
            }

            --open_parenthesis;
            expect_value = false;
        }
        else if ( isAtomicExpr( peek() ) )
        {
            auto expr = parseAtomicExpression();
            if ( !expr )
            {
                throw std::runtime_error("Error: Could not parse atomic expression");
            }
            expr_stack.push_back( std::move( expr ) );
            expect_value = false;
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

    // Collapse the entire stack into a single Expression
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

    if ( last_op.type == TOKEN_TYPE::TOKEN_UNARY_OPERATOR )
    {
        expr_stack.push_back( std::make_unique<UnaryExpression>(
                std::string( last_op.value ),
                std::move( expr_right ) ) );
        return;
    }

    if ( expr_stack.empty() )
    {
        throw std::runtime_error("Error: Unexpected operator.");
    }
    auto expr_left = std::move( expr_stack.back() );
    expr_stack.pop_back();

    expr_stack.push_back( std::make_unique<BinaryExpression>( 
            std::string( last_op.value ), 
            std::move( expr_left ), 
            std::move( expr_right ) ) );
}