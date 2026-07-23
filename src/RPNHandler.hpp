#ifndef RPNHANDLER_HPP
#define RPNHANDLER_HPP

#include"expression.hpp"

#include<string>
#include<vector>
#include<memory>
#include<unordered_map>

class IParserContext;
struct Token;

class RPNHandler
{
private:
    static inline const std::unordered_map<std::string, int> operator_precedence {
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

    static constexpr int unary_precedence = 3;
    
    void handleOpRPN(IParserContext& pctx,
                    std::vector<Token>& operator_stack, 
                    std::vector<std::unique_ptr<Expression>>& expr_stack);
    void handleClosingParenthesisRPN(IParserContext& pctx,
                    std::vector<Token>& operator_stack, 
                    std::vector<std::unique_ptr<Expression>>& expr_stack);
    void makeBinExprRPN(std::vector<Token>& operator_stack, 
                    std::vector<std::unique_ptr<Expression>>& expr_stack);
public:
    std::unique_ptr<Expression> parseRPN(IParserContext& pctx);
    std::unique_ptr<Expression> parseFunctionCallRPN(IParserContext& pctx);
};


#endif