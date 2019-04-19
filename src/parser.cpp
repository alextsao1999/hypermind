//
// Created by 曹顺 on 2019/2/11.
//

#include "parser.h"
#include "ast.h"
#include "symbol.h"
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
        if (tok.type == TokenType::Identifier || tok.type == TokenType::KeywordThis) {
            if (mLexer.Match(TokenType::LeftParen)) {
                leaf = make_ptr(ASTFunctionCall, tok, ParseArgList());
                mLexer.Consume(TokenType::RightParen);
            } else {
                leaf = make_ptr(ASTVariable, tok);
            }
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
            // TODO Error 不存在的Primary ??
            return nullptr;
        }
        while (true) {
            TokenType type = mLexer.PeekTokenType();
            if (type == TokenType::LeftParen) {
                mLexer.Consume();
                leaf = make_ptr(ASTArgPostfix, leaf, ParseArgList());
                mLexer.Consume(TokenType::RightParen);
            } else if (type == TokenType::LeftBracket) {
                mLexer.Consume();
                leaf = make_ptr(ASTSubscriptPostfix, leaf, ParseArgList());
                mLexer.Consume(TokenType::RightBracket);
            } else if (type == TokenType::Dot) {
                mLexer.Consume();
                Token name = mLexer.Read();
                if (mLexer.Match(TokenType::LeftParen)) {
                    leaf = make_ptr(ASTMethodPostfix, leaf, name, ParseArgList());
                    mLexer.Consume(TokenType::RightParen);
                } else {
                    leaf = make_ptr(ASTDotPostfix, leaf, name);
                }
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
            Token op = mLexer.Peek();
            Precedence nextOp = opPrecs[(HMInteger) op.type];
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
        if (mLexer.PeekTokenType() != TokenType::LeftParen) {
            ast->name = mLexer.Read();
        }
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
        ASTClassStmtPtr classPtr = make_ptr(ASTClassStmt);
        Token name = mLexer.Read();
        classPtr->name = name;
        if (mLexer.Match(TokenType::Colon)) {
            classPtr->super = mLexer.Read();
        }
        HMBool inStatic = false;
        SkipDelimiter();
        if (mLexer.Match(TokenType::LeftBrace)) {
            while (true) {
                SkipDelimiter();
                Token token = mLexer.Read();
                switch (token.type) {
                    case TokenType::Add:
                    case TokenType::Sub:
                    case TokenType::Mul:
                    case TokenType::Div:
                    case TokenType::Arrow:
                    case TokenType::Not:
                    case TokenType::Equal:
                    case TokenType::NotEqual:
                    case TokenType::Greater:
                    case TokenType::Less:
                    case TokenType::GreaterEqual:
                    case TokenType::LessEqual:
                    case TokenType::Or:
                    case TokenType::LogicOr:
                    case TokenType::And:
                    case TokenType::LogicAnd:
                    case TokenType::Mod:
                    case TokenType::At:
                    case TokenType::Colon:
                    case TokenType::Identifier:
                    case TokenType::KeywordNew:
                        {
                            ASTMethodStmtPtr method = make_ptr(ASTMethodStmt);

                            // Method
                            if (token.type == TokenType::KeywordNew) {
                                method->name = Signature(SignatureType::Constructor);
                            } else {
                                method->name = Signature(SignatureType::Method, token.start, token.length);
                            }

                            if (mLexer.Match(TokenType::Assign)) {
                                // Setter
                                method->name = Signature(SignatureType::Setter, token.start, token.length);
                            }

                            if (mLexer.Match(TokenType::LeftParen)) {
                                method->params = ParseParamList();
                                mLexer.Consume(TokenType::RightParen);
                            } else {
                                method->name = Signature(SignatureType::Getter, token.start, token.length);
                            }

                            method->body = ParseBlock();
                            if (inStatic)
                                classPtr->statics.push_back(method);
                            else
                                classPtr->methods.push_back(method);
                        }
                        inStatic = false;
                        break;
                    case TokenType::LeftBracket:
                        {
                            ASTMethodStmtPtr method = make_ptr(ASTMethodStmt);
                            method->params = ParseParamList();
                            mLexer.Consume(TokenType::RightBracket);
                            if (mLexer.Match(TokenType::Assign)) {
                                // Subscript Setter
                                method->name = Signature(SignatureType::SubscriptSetter);
                                mLexer.Match(TokenType::LeftParen);
                                method->params->elements.push_back(make_ptr(ASTParamStmt, mLexer.Read(), nullptr));
                                mLexer.Match(TokenType::RightParen);
                            } else {
                                // Subscript
                                method->name = Signature(SignatureType::Subscript);
                            }
                            method->body = ParseBlock();
                            if (inStatic)
                                classPtr->statics.push_back(method);
                            else
                                classPtr->methods.push_back(method);
                        }
                        inStatic = false;
                        break;
                    case TokenType::KeywordVar: {
                        ASTVarStmtPtr ast = make_ptr(ASTVarStmt);
                        ast->identifier = mLexer.Read();
                        if (mLexer.Match(TokenType::Assign)) {
                            ast->value = ParseExpression();
                        }
                        if (inStatic)
                            classPtr->statics.push_back(ast);
                        else
                            classPtr->fields.push_back(ast);
                    }
                        inStatic = false;
                        break;
                    case TokenType::KeywordStatic:
                        inStatic = true;
                        break;
                    case TokenType::KeywordPublic:

                        break;
                    case TokenType::RightBrace:
                        return classPtr;
                    default:
                        // Error
                        break;
                }
            }
        } else {
            // Error 要以大括号开始
        }
        return classPtr;
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
        TokenType next = mLexer.PeekTokenType();
        if (next == TokenType::RightParen || next == TokenType::RightBracket)
            return list;
        do {
            ASTParamStmtPtr &&var = make_ptr(ASTParamStmt);
            var->identifier = mLexer.Read();
            if (var->identifier.type != TokenType::Identifier) {
                // TODO Error Missing Idenifier
            }
            list->elements.push_back(var);
        } while (mLexer.Match(TokenType::Comma));
        return list;
    }

    ASTListPtr Parser::ParseArgList() {
        ASTListPtr list = make_ptr(ASTList);
        TokenType next = mLexer.PeekTokenType();
        if (next == TokenType::RightParen || next == TokenType::RightBracket)
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