//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"

namespace hypermind{

    // Unary ::= Primary |  ! Primary | ++ Primary | Primary ++
    ASTExprPtr Parser::ParseUnary() {
        return ParsePrimary();
    }

    // Primary ::= Literal | Varible
    ASTExprPtr Parser::ParsePrimary() {
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

    ASTExprPtr Parser::ParseExpression() {
        ASTExprPtr lhs = ParseUnary();
        return ParseBinaryOp(lhs, 0);
    }

    // BinaryOp ::= ('+'  Unary)
    ASTExprPtr Parser::ParseBinaryOp(ASTExprPtr lhs, HMInteger prec) {
        while (true) {
            TokenType op = mLexer.PeekTokenType();
            Precedence nextOp = opPrecs[(HMInteger) op];
            if (nextOp.prec == -1) // 没有符号优先级信息
                return lhs;
            if (prec > nextOp.prec)
                break;
            mLexer.Consume();
            ASTBinaryPtr lhsBin = make_ptr(ASTBinary);
            lhsBin->mOp.mType = op;
            lhsBin->mLHS = lhs;
            lhsBin->mRHS = ParseBinaryOp(ParseUnary(), nextOp.association ? nextOp.prec + 1 : nextOp.prec);
            lhs = lhsBin;
        }
        return lhs;
    }

}