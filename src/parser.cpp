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

    ASTStmtPtr Parser::ParseVarStmt() {
        return hypermind::ASTStmtPtr();
    }

    ASTStmtPtr Parser::ParseFunctionStmt() {
        return hypermind::ASTStmtPtr();
    }

    ASTStmtPtr Parser::ParseIfStmt() {
        return hypermind::ASTStmtPtr();
    }

    ASTStmtPtr Parser::ParseWhileStmt() {
        return hypermind::ASTStmtPtr();
    }

    ASTStmtPtr Parser::ParseBreakStmt() {
        return hypermind::ASTStmtPtr();
    }

    ASTStmtPtr Parser::ParseContinueStmt() {
        return hypermind::ASTStmtPtr();
    }

    ASTStmtPtr Parser::ParseReturnStmt() {
        return hypermind::ASTStmtPtr();
    }

    ASTStmtPtr Parser::ParseClassStmt() {
        return hypermind::ASTStmtPtr();
    }

    ASTBlockPtr Parser::ParseBlock() {
        ASTBlockPtr blockPtr = make_ptr(ASTBlock);
        if (mLexer.PeekTokenType() == TokenType::LeftBrace) {
            mLexer.Consume();
            ASTNodePtr stmt;
            while ((stmt = ParseProgram()) != nullptr) {

                blockPtr->addStmt(std::move(stmt));
                if (mLexer.PeekTokenType() == TokenType::RightBrace)
                    break;
            }
            
        } else {

        }

        return blockPtr;
    }

    ASTNodePtr Parser::ParseProgram() {
        TokenType tok = mLexer.ReadTokenType();
        switch (tok) {
            case TokenType::KeywordIf:
                return ParseIfStmt();
            case TokenType::KeywordWhile:
                return ParseWhileStmt();
            case TokenType::KeywordBreak:
                break;
            case TokenType::KeywordContinue:
                break;
            case TokenType::KeywordFor:
                break;
            case TokenType::keywordClass:
                break;
            case TokenType::KeywordFunction:
                break;
            case TokenType::KeywordReturn:
                break;
            case TokenType::KeywordVar:
                break;
            case TokenType::KeywordThis:
                break;
            case TokenType::KeywordSuper:
                break;
            case TokenType::KeywordTrue:
                break;
            case TokenType::KeywordFalse:
                break;
            case TokenType::KeywordStatic:
                break;
            default:
                // 错误
                break;
        }
    }

}