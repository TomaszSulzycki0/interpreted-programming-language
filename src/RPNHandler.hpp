#ifndef RPNHANDLER_HPP
#define RPNHANDLER_HPP

#include"expression.hpp"

#include<string>
#include<vector>
#include<memory>
#include<unordered_map>

class IParserContext;
struct Token;

// Helper class for the Parser used for parsing complex expressions
class RPNHandler
{
private:
    static inline const std::unordered_map<std::string, int> operator_precedence {
        { "or", 0 }, 
        { "and", 0 }, 
        { "<", 0 }, 
        { ">", 0 }, 
        { "==", 0 }, 
        { "!=", 0 }, 
        { ">=", 0 }, 
        { "<=", 0 }, 
        { "-", 1 }, 
        { "+", 1 },
        { "*", 2 },
        { "%", 2 },
        { "/", 2 }
    };

    static constexpr int unary_precedence = 3;
    
    void handleOpRPN(IParserContext& pctx,
                    std::vector<Token>& operator_stack, 
                    std::vector<std::unique_ptr<Expression>>& expr_stack);
    void handleClosingParenthesisRPN(std::vector<Token>& operator_stack, 
                    std::vector<std::unique_ptr<Expression>>& expr_stack);
    void makeBinExprRPN(std::vector<Token>& operator_stack, 
                    std::vector<std::unique_ptr<Expression>>& expr_stack);
public:

    // Default method for parsing expressions
    std::unique_ptr<Expression> parseRPN(IParserContext& pctx);

    // Method for parsing conditions in if / while statements
    std::unique_ptr<Expression> parseRPNCondition(IParserContext& pctx);

    // Method for parsing function call arguments
    std::unique_ptr<Expression> parseFunctionCallRPN(IParserContext& pctx, bool& is_fn_call_end);
};


#endif