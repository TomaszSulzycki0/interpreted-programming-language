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
    void makeBinExprRPN(std::vector<Token>& operator_stack, 
                    std::vector<std::unique_ptr<Expression>>& expr_stack);
public:
    std::unique_ptr<Expression> parseRPN(IParserContext& pctx);
};


#endif