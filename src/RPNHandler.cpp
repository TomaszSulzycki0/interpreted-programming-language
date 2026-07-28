#include"RPNHandler.hpp"
#include"tokenizer.hpp"
#include"parser.hpp"


std::unique_ptr<Expression> RPNHandler::parseRPN(IParserContext& pctx)
{
    std::vector<Token> operator_stack {};
    std::vector<std::unique_ptr<Expression>> expr_stack {};

    // Counter for validating that ever open parenthesis is closed
    int unclosed_parenthesis = 0;

    // Flag used for identifying unary operators
    // Set to true if next token is expected to be a value, false otherwise
    // ...) - x -> minus is not unary
    // ...( - x -> minus is unary
    bool expect_value = true;

    while ( !pctx.isDone() )
    {
        if ( pctx.check( TOKEN_TYPE::TOKEN_OPERATOR ) && expect_value )
        {
            // There should be a procedure for handling the operator stack here
            // For now assume that unary operators have the highest possible precedence
            Token op = pctx.advance();
            op.type = TOKEN_TYPE::TOKEN_UNARY_OPERATOR;

            operator_stack.push_back( op );
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_OPERATOR ) )
        {
            handleOpRPN( pctx, operator_stack, expr_stack );
            expect_value = true;
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN ) )
        {
            operator_stack.push_back( pctx.advance() );
            expect_value = true;
            ++unclosed_parenthesis;
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE ) )
        {
            if ( unclosed_parenthesis < 1 )
            {
                throw std::runtime_error("Error: Unexpected closing parenthesis without opening counterpart.");
            }

            // Eat parenthesis
            pctx.advance();

            handleClosingParenthesisRPN( operator_stack, expr_stack );

            --unclosed_parenthesis;
            expect_value = false;
        }
        else if ( pctx.isAtomicExpr( pctx.peek() ) )
        {
            auto expr = pctx.parseAtomicExpression();
            if ( !expr )
            {
                throw std::runtime_error("Error: Could not parse atomic expression");
            }
            expr_stack.push_back( std::move( expr ) );
            expect_value = false;
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_SEMICOLON ) )
        {
            break;
        }
        else
        {
            throw std::runtime_error("Error: Invalid expression");
        }
    }

    if ( unclosed_parenthesis != 0 )
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

std::unique_ptr<Expression> RPNHandler::parseFunctionCallRPN(IParserContext& pctx, bool& is_fn_call_end)
{
    std::vector<Token> operator_stack {};
    std::vector<std::unique_ptr<Expression>> expr_stack {};

    // Counter for validating that ever open parenthesis is closed
    std::size_t unclosed_parenthesis { 1 };

    // Flag used for identifying unary operators
    // Set to true if next token is expected to be a value, false otherwise
    // ...) - x -> minus is not unary
    // ...( - x -> minus is unary
    bool expect_value = true;

    while ( !pctx.isDone() )
    {
        if ( pctx.check( TOKEN_TYPE::TOKEN_OPERATOR ) && expect_value )
        {
            // There should be a procedure for handling the operator stack here
            // For now assume that unary operators have the highest possible precedence
            Token op = pctx.advance();
            op.type = TOKEN_TYPE::TOKEN_UNARY_OPERATOR;

            operator_stack.push_back( op );
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_OPERATOR ) )
        {
            handleOpRPN( pctx, operator_stack, expr_stack );
            expect_value = true;
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN ) )
        {
            operator_stack.push_back( pctx.advance() );
            expect_value = true;
            ++unclosed_parenthesis;
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE ) )
        {
            if ( unclosed_parenthesis < 1 )
            {
                throw std::runtime_error("Error: Unexpected closing parenthesis without opening counterpart.");
            }

            // Eat parenthesis
            pctx.advance();

            handleClosingParenthesisRPN( operator_stack, expr_stack );

            --unclosed_parenthesis;
            expect_value = false;

            if ( unclosed_parenthesis == 0 )
            {
                // End of function call
                is_fn_call_end = true;
                break;
            }
        }
        else if ( pctx.isAtomicExpr( pctx.peek() ) )
        {
            auto expr = pctx.parseAtomicExpression();
            if ( !expr )
            {
                throw std::runtime_error("Error: Could not parse atomic expression");
            }
            expr_stack.push_back( std::move( expr ) );
            expect_value = false;
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_COMMA ) )
        {
            pctx.advance();
            break;
        }
        else
        {
            throw std::runtime_error("Error: Invalid expression");
        }
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

std::unique_ptr<Expression> RPNHandler::parseRPNCondition(IParserContext& pctx)
{
    std::vector<Token> operator_stack {};
    std::vector<std::unique_ptr<Expression>> expr_stack {};

    // Counter for validating that ever open parenthesis is closed
    std::size_t unclosed_parenthesis { 1 };

    // Flag used for identifying unary operators
    // Set to true if next token is expected to be a value, false otherwise
    // ...) - x -> minus is not unary
    // ...( - x -> minus is unary
    bool expect_value = true;

    while ( !pctx.isDone() )
    {
        if ( pctx.check( TOKEN_TYPE::TOKEN_OPERATOR ) && expect_value )
        {
            // There should be a procedure for handling the operator stack here
            // For now assume that unary operators have the highest possible precedence
            Token op = pctx.advance();
            op.type = TOKEN_TYPE::TOKEN_UNARY_OPERATOR;

            operator_stack.push_back( op );
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_OPERATOR ) )
        {
            handleOpRPN( pctx, operator_stack, expr_stack );
            expect_value = true;
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN ) )
        {
            operator_stack.push_back( pctx.advance() );
            expect_value = true;
            ++unclosed_parenthesis;
        }
        else if ( pctx.check( TOKEN_TYPE::TOKEN_PARENTHESIS_CLOSE ) )
        {
            if ( unclosed_parenthesis < 1 )
            {
                throw std::runtime_error("Error: Unexpected closing parenthesis without opening counterpart.");
            }

            // Eat parenthesis
            pctx.advance();

            handleClosingParenthesisRPN( operator_stack, expr_stack );

            --unclosed_parenthesis;
            expect_value = false;

            if ( unclosed_parenthesis == 0 )
            {
                break;
            }
        }
        else if ( pctx.isAtomicExpr( pctx.peek() ) )
        {
            auto expr = pctx.parseAtomicExpression();
            if ( !expr )
            {
                throw std::runtime_error("Error: Could not parse atomic expression");
            }
            expr_stack.push_back( std::move( expr ) );
            expect_value = false;
        }
        else
        {
            throw std::runtime_error("Error: Invalid expression");
        }
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

void RPNHandler::handleOpRPN(IParserContext& pctx, std::vector<Token>& operator_stack, std::vector<std::unique_ptr<Expression>>& expr_stack)
{
    Token op = pctx.advance();

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
        
        int last_op_precedence = ( last_op.type == TOKEN_TYPE::TOKEN_UNARY_OPERATOR ) ? unary_precedence : _it->second;

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

void RPNHandler::handleClosingParenthesisRPN(std::vector<Token>& operator_stack, std::vector<std::unique_ptr<Expression>>& expr_stack)
{
    while ( !operator_stack.empty() )
    {   
        // RPN logic

        auto last_op = operator_stack.back();
        operator_stack.pop_back();
        
        if ( last_op.type == TOKEN_TYPE::TOKEN_PARENTHESIS_OPEN )
        {
            // ( ) - fine.
            break;
        }

        auto expr_right = std::move( expr_stack.back() );
        expr_stack.pop_back();

        if ( expr_stack.empty() )
        {
            // ( x ) - fine. Leave 'x' on the stack
            expr_stack.push_back( std::move( expr_right) );
            break;
        }
        auto expr_left = std::move( expr_stack.back() );
        expr_stack.pop_back();

        expr_stack.push_back( std::make_unique<BinaryExpression>( std::string(last_op.value), std::move( expr_left ), std::move( expr_right ) ) );
    }
}

void RPNHandler::makeBinExprRPN( std::vector<Token>& operator_stack, std::vector<std::unique_ptr<Expression>>& expr_stack)
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