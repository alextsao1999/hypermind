//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"

namespace hypermind {

    // Unary ::= Primary |  "!" Primary | "++" Primary | Primary "++"
    ASTExprPtr Parser::ParseUnary() {
        return ParsePrimary();
    }

    // Primary ::= Literal | Varible | true | false
    ASTExprPtr Parser::ParsePrimary() {
        Token tok = mLexer.Read();
        if (tok.mType == TokenType::Identifier) {
            ASTVariablePtr astVariablePtr = make_ptr(ASTVariable);
            astVariablePtr->mVar = tok;
            return astVariablePtr;
        } else if (tok.mType == TokenType::String) {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->mValue = tok.mValue;
            return astLiteralPtr;
        } else if (tok.mType == TokenType::KeywordTrue) {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->mValue.type = ValueType::True;
            return astLiteralPtr;
        } else if (tok.mType == TokenType::KeywordFalse) {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->mValue.type = ValueType::False;
            return astLiteralPtr;
        } else if (tok.mType == TokenType::Number) {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->mValue = tok.mValue;
            return astLiteralPtr;
        } else {
            // TODO 不存在的Primary ??
            return nullptr;
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
        mLexer.Consume(); // Consume function
        ASTFunctionStmtPtr ast = make_ptr(ASTFunctionStmt);
        ast->mName = mLexer.Read();
        if (!mLexer.Match(TokenType::LeftParen)) {
            // TODO ErrorReport 缺少左括号
        }
        ast->mParams = ParseParamList();
        if (!mLexer.Match(TokenType::RightParen)) {
            // TODO ErrorReport 缺少右括号
        }
        ast->mBody = ParseBlock();
        return ast;
    }

    ASTStmtPtr Parser::ParseIfStmt() {
        mLexer.Consume(); // Consume If
        ASTIfStmtPtr ast = make_ptr(ASTIfStmt);

        if (!mLexer.Match(TokenType::LeftParen)) {
            // TODO ErrorReport 缺少左括号
        }
        ast->mCondition = ParseExpression();
        if (!mLexer.Match(TokenType::RightParen)) {
            // TODO ErrorReport 缺少右括号
        }
        ast->mThen = ParseBlock();
        if (mLexer.Match(TokenType::KeywordElse)) {
            ast->mElse = ParseBlock();
        }

        return ast;
    }

    ASTStmtPtr Parser::ParseWhileStmt() {
        mLexer.Consume(); // Consume while
        ASTWhileStmtPtr ast = make_ptr(ASTWhileStmt);
        if (!mLexer.Match(TokenType::LeftParen)) {
            // TODO ErrorReport 缺少左括号
        }
        ast->mCondition = ParseExpression();
        if (!mLexer.Match(TokenType::RightParen)) {
            // TODO ErrorReport 缺少右括号
        }
        ast->mBlock = ParseBlock();
        return ast;
    }

    ASTStmtPtr Parser::ParseBreakStmt() {
        mLexer.Consume();
        return make_ptr(ASTBreakStmt);
    }

    ASTStmtPtr Parser::ParseContinueStmt() {
        mLexer.Consume();
        return make_ptr(ASTContinueStmt);
    }

    ASTStmtPtr Parser::ParseReturnStmt() {
        ASTReturnStmtPtr ast = make_ptr(ASTReturnStmt);
        // FIXME  注意! 可能是空返回值
        ast->mRetval = ParseExpression();
        return ast;
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
                if (mLexer.Match(TokenType::RightBrace))
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

    /**
     * ParamList ::= ID {"," ID}
     * @return
     */
    ASTNodePtr Parser::ParseParamList() {
        ASTListPtr list = make_ptr(ASTList);
        if (mLexer.PeekTokenType() == TokenType::RightParen)
            return list;
        do {
            ASTVarStmtPtr &&var = make_ptr(ASTVarStmt);
            var->mIdentifier = mLexer.Read();
            list->elements.push_back(var);
        } while (mLexer.Match(TokenType::Comma));
        return list;
    }

    ASTNodePtr Parser::ParseArgList() {
        ASTListPtr list = make_ptr(ASTList);
        if (mLexer.PeekTokenType() == TokenType::RightParen)
            return list;
        do {
            ASTExprPtr &&ptr = ParseExpression();
            list->elements.push_back(ptr);
        } while (mLexer.Match(TokenType::Comma));
        return list;
    }

}