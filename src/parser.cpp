//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"
#include "ast.h"
#define SkipDelimiter() while (mLexer.PeekTokenType() == TokenType::Delimiter) mLexer.Consume();

namespace hypermind {

    // Unary ::= Primary |  "!" Primary | "++" Primary | Primary "++"
    ASTNodePtr Parser::ParseUnary() {
        TokenType next = mLexer.PeekTokenType();
        if (next == TokenType::Not) {
            return make_ptr(ASTNotExpr, ParseUnary());
        } else if (next == TokenType::Sub) {
            return make_ptr(ASTNegativeExpr, ParseUnary());
        }
        return ParsePrimary();
    }

    // Primary ::= Literal | Varible | true | false
    ASTNodePtr Parser::ParsePrimary() {
        Token tok = mLexer.Read();
        ASTNodePtr leaf;
        if (tok.type == TokenType::Identifier) {
            leaf = make_ptr(ASTVariable, tok);
        } else if (tok.type == TokenType::String || tok.type == TokenType::Number) {
            leaf = make_ptr(ASTLiteral, tok.value);
        } else if (tok.type == TokenType::KeywordTrue) {
            leaf = make_ptr(ASTLiteral, Value(ValueType::True));
        } else if (tok.type == TokenType::KeywordFalse) {
            leaf = make_ptr(ASTLiteral, Value(ValueType::False));
        } else if (tok.type == TokenType::LeftParen) {
            leaf = ParseExpression();
            mLexer.Consume(TokenType::RightParen);
        } else {
            // TODO 不存在的Primary ??
            return nullptr;
        }
        while (true) {
            TokenType type = mLexer.PeekTokenType();
            if (type == TokenType::LeftParen) {
                mLexer.Consume();
                leaf = make_ptr(ASTArgPostfix, leaf, ParseArgList());
                mLexer.Consume(TokenType::RightParen);
            } else if (type == TokenType::Dot) {
                mLexer.Consume();
                leaf = make_ptr(ASTDotPostfix, leaf, mLexer.Read());
            } else {
                break;
            }
        }
        return leaf;
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
            lhs = make_ptr(ASTBinary, op, lhs,
                           ParseBinaryOp(ParseUnary(), nextOp.association ? nextOp.prec + 1 : nextOp.prec));
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
        mLexer.Consume(TokenType::LeftParen);
        ast->params = ParseParamList();
        mLexer.Consume(TokenType::RightParen);
        ast->body = ParseBlock();
        return ast;
    }

    ASTNodePtr Parser::ParseIfStmt() {
        mLexer.Consume(); // Consume If
        ASTIfStmtPtr ast = make_ptr(ASTIfStmt);
        mLexer.Consume(TokenType::LeftParen);
        ast->condition = ParseExpression();
        mLexer.Consume(TokenType::RightParen);
        ast->thenBlock = ParseBlock();
        if (mLexer.Match(TokenType::KeywordElse)) {
            ast->elseBlock = ParseBlock();
        }

        return ast;
    }

    ASTNodePtr Parser::ParseWhileStmt() {
        mLexer.Consume(); // Consume while
        ASTWhileStmtPtr ast = make_ptr(ASTWhileStmt);
        mLexer.Consume(TokenType::LeftParen);
        ast->condition = ParseExpression();
        mLexer.Consume(TokenType::RightParen);
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
        mLexer.Consume();
        ASTReturnStmtPtr ast = make_ptr(ASTReturnStmt);
        if (!mLexer.Match(TokenType::Delimiter)) {
            ast->retvalue = ParseExpression();
        }
        return ast;
    }

    ASTNodePtr Parser::ParseClassStmt() {
        mLexer.Consume();
        return hypermind::ASTNodePtr();
    }

    ASTBlockPtr Parser::ParseBlock() {
        ASTBlockPtr blockPtr = make_ptr(ASTBlock);
        SkipDelimiter();
        if (mLexer.PeekTokenType() == TokenType::LeftBrace) {
            mLexer.Consume();
            ASTNodePtr stmt;
            while ((stmt = ParseStmt()) != nullptr) {
                blockPtr->addStmt(std::move(stmt));
                if (mLexer.Match(TokenType::RightBrace))
                    break;
            }
        } else {
            blockPtr->addStmt(std::move(ParseStmt()));
        }
        return blockPtr;
    }

    ASTNodePtr Parser::ParseStmt() {
        SkipDelimiter();
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
        SkipDelimiter();
        return ptr;
    }

    /**
     * ParamList ::= ID {"," ID}
     * @return
     */
    ASTListPtr Parser::ParseParamList() {
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

    ASTListPtr Parser::ParseArgList() {
        ASTListPtr list = make_ptr(ASTList);
        if (mLexer.PeekTokenType() == TokenType::RightParen)
            return list;
        do {
            ASTNodePtr &&ptr = ParseExpression();
            list->elements.push_back(ptr);
        } while (mLexer.Match(TokenType::Comma));
        return list;
    }

    ASTListPtr Parser::ParseProgram() {
        ASTListPtr list = make_ptr(ASTList);
        while (!mLexer.mEof) {
            list->addItem(ParseStmt());
        }
        return list;
    }

}