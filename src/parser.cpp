//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"

namespace hypermind{

    ASTExprPtr Parser::parseUnary() {
        return parsePrimary();
    }

    ASTExprPtr Parser::parsePrimary() {
        Token tok = mLexer.Read();
        if (tok.mType == TokenType::Identifier) {
            ASTVariablePtr astVariablePtr = make_ptr(ASTVariable);
            astVariablePtr->mVar = tok;
            return astVariablePtr;
        } else {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->mValue = tok.mValue;
            return astLiteralPtr;
        }

    }

    ASTExprPtr Parser::parseBinary() {

        return hypermind::ASTExprPtr();
    }

    ASTExprPtr Parser::parseBinOp(ASTExprPtr lhs, HMInteger prec) {
        return hypermind::ASTExprPtr();
    }

}