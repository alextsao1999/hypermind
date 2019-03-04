//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"

namespace hypermind{

    // Unary ::= Primary |  ! Primary | ++ Primary | Primary ++
    ASTExprPtr Parser::parseUnary() {
        return parsePrimary();
    }

    // Primary ::= Literal | Varible
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

    ASTExprPtr Parser::parseExpression() {
        ASTExprPtr lhs = parseUnary();
        return parseBinaryOp(lhs, 0);
    }

    // BinaryOp ::= ('+'  Unary)
    ASTExprPtr Parser::parseBinaryOp(ASTExprPtr lhs, HMInteger prec) {
        while (true) {
            TokenType op = mLexer.PeekTokenType();
            Precedence nextOp = opPrecs[(HMInteger) op];
            if (nextOp.prec == -1) // 没有符号优先级信息 并不是 Expression
                return lhs;
            mLexer.Consume();
            ASTExprPtr rhs;
            if (nextOp.prec < prec)
                break;

            ASTBinaryPtr &&lhsBin = make_ptr(ASTBinary);
            lhsBin->mLHS = lhs;
            if (nextOp.association) { // 真为 左结合
                lhsBin->mRHS = parseBinaryOp(lhs, nextOp.prec + 1);
            } else {
                lhsBin->mRHS = parseBinaryOp(lhs, nextOp.prec);
            }
            lhs = lhsBin;
        }
        return lhs;
    }

}