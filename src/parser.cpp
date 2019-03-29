//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"
#include "ast.h"

namespace hypermind {

    // Unary ::= Primary |  "!" Primary | "++" Primary | Primary "++"
    ASTNodePtr Parser::ParseUnary() {
        return ParsePrimary();
    }

    // Primary ::= Literal | Varible | true | false
    ASTNodePtr Parser::ParsePrimary() {
        Token tok = mLexer.Read();
        if (tok.type == TokenType::Identifier) {
            ASTVariablePtr astVariablePtr = make_ptr(ASTVariable);
            astVariablePtr->var = tok;
            return astVariablePtr;
        } else if (tok.type == TokenType::String) {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->value = tok.value;
            return astLiteralPtr;
        } else if (tok.type == TokenType::KeywordTrue) {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->value.type = ValueType::True;
            return astLiteralPtr;
        } else if (tok.type == TokenType::KeywordFalse) {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->value.type = ValueType::False;
            return astLiteralPtr;
        } else if (tok.type == TokenType::Number) {
            ASTLiteralPtr astLiteralPtr = make_ptr(ASTLiteral);
            astLiteralPtr->value = tok.value;
            return astLiteralPtr;
        } else {
            // TODO 不存在的Primary ??
            return nullptr;
        }
    }

    ASTNodePtr Parser::ParseExpression() {
//        ASTExprPtr lhs = ;
        return ParseBinaryOp(ParseUnary(), 0);
    }

    // BinaryOp ::= ('+'  Unary)
    ASTNodePtr Parser::ParseBinaryOp(ASTNodePtr lhs, HMInteger prec) {
        while (true) {
            TokenType op = mLexer.PeekTokenType();
            Precedence nextOp = opPrecs[(HMInteger) op];
            if (nextOp.prec == -1) // 没有符号优先级信息
                return lhs;
            if (prec > nextOp.prec)
                break;
            mLexer.Consume();
            ASTBinaryPtr lhsBin = make_ptr(ASTBinary);
            lhsBin->op.type = op;
            lhsBin->lhs = lhs;
            lhsBin->rhs = ParseBinaryOp(ParseUnary(), nextOp.association ? nextOp.prec + 1 : nextOp.prec);
            lhs = lhsBin;
        }
        return lhs;
    }

    ASTNodePtr Parser::ParseVarStmt() {
        mLexer.Consume();
        ASTVarStmtPtr ast = make_ptr(ASTVarStmt);
        ast->identifier = mLexer.Read();
        if (mLexer.Match(TokenType::Assign)) {
            ast->value = ParseExpression();
        }
        return ast;
    }

    ASTNodePtr Parser::ParseFunctionStmt() {
        mLexer.Consume(); // Consume function
        ASTFunctionStmtPtr ast = make_ptr(ASTFunctionStmt);
        ast->name = mLexer.Read();
        if (!mLexer.Match(TokenType::LeftParen)) {
            // TODO ErrorReport 缺少左括号
        }
        ast->params = ParseParamList();
        if (!mLexer.Match(TokenType::RightParen)) {
            // TODO ErrorReport 缺少右括号
        }
        ast->body = ParseBlock();
        return ast;
    }

    ASTNodePtr Parser::ParseIfStmt() {
        mLexer.Consume(); // Consume If
        ASTIfStmtPtr ast = make_ptr(ASTIfStmt);

        if (!mLexer.Match(TokenType::LeftParen)) {
            // TODO ErrorReport 缺少左括号
        }
        ast->condition = ParseExpression();
        if (!mLexer.Match(TokenType::RightParen)) {
            // TODO ErrorReport 缺少右括号
        }
        ast->thenBlock = ParseBlock();
        if (mLexer.Match(TokenType::KeywordElse)) {
            ast->elseBlock = ParseBlock();
        }

        return ast;
    }

    ASTNodePtr Parser::ParseWhileStmt() {
        mLexer.Consume(); // Consume while
        ASTWhileStmtPtr ast = make_ptr(ASTWhileStmt);
        if (!mLexer.Match(TokenType::LeftParen)) {
            // TODO ErrorReport 缺少左括号
        }
        ast->condition = ParseExpression();
        if (!mLexer.Match(TokenType::RightParen)) {
            // TODO ErrorReport 缺少右括号
        }
        ast->block = ParseBlock();
        return ast;
    }

    ASTNodePtr Parser::ParseBreakStmt() {
        mLexer.Consume();
        return make_ptr(ASTBreakStmt);
    }

    ASTNodePtr Parser::ParseContinueStmt() {
        mLexer.Consume();
        return make_ptr(ASTContinueStmt);
    }

    ASTNodePtr Parser::ParseReturnStmt() {
        ASTReturnStmtPtr ast = make_ptr(ASTReturnStmt);
        // FIXME  注意! 可能是空返回值
        ast->retvalue = ParseExpression();
        return ast;
    }

    ASTNodePtr Parser::ParseClassStmt() {
        return hypermind::ASTNodePtr();
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
            ASTParamStmtPtr &&var = make_ptr(ASTParamStmt);
            var->identifier = mLexer.Read();
            list->elements.push_back(var);
        } while (mLexer.Match(TokenType::Comma));
        return list;
    }

    ASTNodePtr Parser::ParseArgList() {
        ASTListPtr list = make_ptr(ASTList);
        if (mLexer.PeekTokenType() == TokenType::RightParen)
            return list;
        do {
            ASTNodePtr &&ptr = ParseExpression();
            list->elements.push_back(ptr);
        } while (mLexer.Match(TokenType::Comma));
        return list;
    }

}