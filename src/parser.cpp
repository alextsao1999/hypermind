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
        mLexer.Consume();
        ASTVarStmtPtr ast = make_ptr(ASTVarStmt);
        ast->mIdentifier = mLexer.Read();
        if (mLexer.Match(TokenType::Assign)) {
            ast->mValue = ParseExpression();
        }
        return ast;
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
            blockPtr->addStmt(std::move(ParseProgram()));
        }
        return blockPtr;
    }

    ASTNodePtr Parser::ParseProgram() {
        TokenType tok = mLexer.PeekTokenType();
        ASTNodePtr ptr;
        switch (tok) {
            case TokenType::KeywordIf:
                ptr = ParseIfStmt();
                break;
            case TokenType::KeywordWhile:
                ptr = ParseWhileStmt();
                break;
            case TokenType::KeywordBreak:
                ptr = ParseBreakStmt();
                break;
            case TokenType::KeywordContinue:
                ptr = ParseContinueStmt();
                break;
            case TokenType::KeywordFor:
                break;
            case TokenType::keywordClass:
                ptr = ParseClassStmt();
                break;
            case TokenType::KeywordFunction:
                ptr = ParseFunctionStmt();
                break;
            case TokenType::KeywordReturn:
                ptr = ParseReturnStmt();
                break;
            case TokenType::KeywordVar:
                ptr = ParseVarStmt();
                break;
            case TokenType::KeywordStatic:
                break;
            default:
                ptr = ParseExpression();
        }
        while (mLexer.PeekTokenType() == TokenType::Delimiter)
            mLexer.Consume();
        return ptr;
    }

}