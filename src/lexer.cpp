//
// Created by 曹顺 on 2019/2/11.
//

#include <iostream>
#include <cstring>
#include "lexer.h"
#include "obj/string.h"

namespace hypermind {

    bool IsSpace(HMChar ch){
        return ch == _HM_C(' ') || ch == _HM_C('\r');
    }
    bool IsNumber(HMChar ch) {
        return ch >= _HM_C('0') && ch <= _HM_C('9');
    }
    bool IsAlpha(HMChar ch) {
        return (ch >= _HM_C('a') && ch <= _HM_C('z')) || (ch >= _HM_C('A') && ch <= _HM_C('Z'));
    }
    bool IsCodeChar(HMChar ch) {
        // 汉字范围 [\u4e00-\u9fa5]
#ifdef HMUNICODE
        return IsAlpha(ch) || ch == _HM_C('_') || (ch >= _HM_C('\u4e00') && ch <= _HM_C('\u9fa5'));
#else
        return IsAlpha(ch) || ch == _HM_C('_');
#endif
    }

    void DumpTokenType(Ostream &os, TokenType type) {
#define TOKEN_DUMP(v) os << _HM_C(v);
        switch (type) {
            case TokenType::End:
                TOKEN_DUMP("<end>");
                break;
            case TokenType::Delimiter:
                TOKEN_DUMP("<delimiter>");
                break;
            case TokenType::Add:
                TOKEN_DUMP("+");
                break;
            case TokenType::Sub:
                TOKEN_DUMP("-");
                break;
            case TokenType::Mul:
                TOKEN_DUMP("*");
                break;
            case TokenType::Div:
                TOKEN_DUMP("/");
                break;
            case TokenType::Identifier:
                TOKEN_DUMP("<identifier>");
            case TokenType::Number:
                TOKEN_DUMP("<number>");
                break;
            case TokenType::String:
                TOKEN_DUMP("<string>");
                break;
            case TokenType::Dot:
                TOKEN_DUMP(".");
                break;
            case TokenType::Comma:
                TOKEN_DUMP(",");
                break;
            case TokenType::LeftParen:
                TOKEN_DUMP("(");
                break;
            case TokenType::RightParen:
                TOKEN_DUMP(")");
                break;
            case TokenType::LeftBracket:
                TOKEN_DUMP("[");
                break;
            case TokenType::RightBracket:
                TOKEN_DUMP("]");
                break;
            case TokenType::LeftBrace:
                TOKEN_DUMP("{");
                break;
            case TokenType::RightBrace:
                TOKEN_DUMP("}");
                break;
            case TokenType::Increase:
                TOKEN_DUMP("++");
                break;
            case TokenType::Decrease:
                TOKEN_DUMP("--");
                break;
            case TokenType::Assign:
                TOKEN_DUMP("=");
                break;
            case TokenType::AddAssign:
                TOKEN_DUMP("+=");
                break;
            case TokenType::SubAssign:
                TOKEN_DUMP("-=");
                break;
            case TokenType::MulAssign:
                TOKEN_DUMP("*=");
                break;
            case TokenType::DivAssign:
                TOKEN_DUMP("/=");
                break;
            case TokenType::ModAssign:
                TOKEN_DUMP("%=");
                break;
            case TokenType::AndAssign:
                TOKEN_DUMP("&=");
                break;
            case TokenType::OrAssign:
                TOKEN_DUMP("|=");
                break;
            case TokenType::XorAssign:
                TOKEN_DUMP("^=");
                break;
            case TokenType::Arrow:
                TOKEN_DUMP("->");
                break;
            case TokenType::Not:
                TOKEN_DUMP("!");
                break;
            case TokenType::Equal:
                TOKEN_DUMP("==");
                break;
            case TokenType::NotEqual:
                TOKEN_DUMP("!=");
                break;
            case TokenType::Greater:
                TOKEN_DUMP(">");
                break;
            case TokenType::Less:
                TOKEN_DUMP("<");
                break;
            case TokenType::GreaterEqual:
                TOKEN_DUMP(">=");
                break;
            case TokenType::LessEqual:
                TOKEN_DUMP("<=");
                break;
            case TokenType::Or:
                TOKEN_DUMP("|");
                break;
            case TokenType::LogicOr:
                TOKEN_DUMP("||");
                break;
            case TokenType::And:
                TOKEN_DUMP("&");
                break;
            case TokenType::LogicAnd:
                TOKEN_DUMP("&&");
                break;
            case TokenType::Mod:
                TOKEN_DUMP("%");
                break;
            case TokenType::At:
                TOKEN_DUMP("@");
                break;
            case TokenType::Colon:
                TOKEN_DUMP(":");
                break;
            default:
                TOKEN_DUMP("<unknown>");
        }
#undef TOKEN_DUMP

    }

    void Token::dump(Ostream &os) const {
#define TOKEN_DUMP(v) name = _HM_C(v);
        String name;
        switch (type) {
            case TokenType::End:
                TOKEN_DUMP("<end>");
                break;
            case TokenType::Delimiter:
                TOKEN_DUMP("<delimiter>");
                break;
            case TokenType::Add:
                TOKEN_DUMP("+");
                break;
            case TokenType::Sub:
                TOKEN_DUMP("-");
                break;
            case TokenType::Mul:
                TOKEN_DUMP("*");
                break;
            case TokenType::Div:
                TOKEN_DUMP("/");
                break;
            case TokenType::Identifier:
            case TokenType::Number:
                name = String(start, length);
                break;
            case TokenType::String:
                break;
            case TokenType::Dot:
                TOKEN_DUMP(".");
                break;
            case TokenType::Comma:
                TOKEN_DUMP(",");
                break;
            case TokenType::LeftParen:
                TOKEN_DUMP("(");
                break;
            case TokenType::RightParen:
                TOKEN_DUMP(")");
                break;
            case TokenType::LeftBracket:
                TOKEN_DUMP("[");
                break;
            case TokenType::RightBracket:
                TOKEN_DUMP("]");
                break;
            case TokenType::LeftBrace:
                TOKEN_DUMP("{");
                break;
            case TokenType::RightBrace:
                TOKEN_DUMP("}");
                break;
            case TokenType::Increase:
                TOKEN_DUMP("++");
                break;
            case TokenType::Decrease:
                TOKEN_DUMP("--");
                break;
            case TokenType::Assign:
                TOKEN_DUMP("=");
                break;
            case TokenType::AddAssign:
                TOKEN_DUMP("+=");
                break;
            case TokenType::SubAssign:
                TOKEN_DUMP("-=");
                break;
            case TokenType::MulAssign:
                TOKEN_DUMP("*=");
                break;
            case TokenType::DivAssign:
                TOKEN_DUMP("/=");
                break;
            case TokenType::ModAssign:
                TOKEN_DUMP("%=");
                break;
            case TokenType::AndAssign:
                TOKEN_DUMP("&=");
                break;
            case TokenType::OrAssign:
                TOKEN_DUMP("|=");
                break;
            case TokenType::XorAssign:
                TOKEN_DUMP("^=");
                break;
            case TokenType::Arrow:
                TOKEN_DUMP("->");
                break;
            case TokenType::Not:
                TOKEN_DUMP("!");
                break;
            case TokenType::Equal:
                TOKEN_DUMP("==");
                break;
            case TokenType::NotEqual:
                TOKEN_DUMP("!=");
                break;
            case TokenType::Greater:
                TOKEN_DUMP(">");
                break;
            case TokenType::Less:
                TOKEN_DUMP("<");
                break;
            case TokenType::GreaterEqual:
                TOKEN_DUMP(">=");
                break;
            case TokenType::LessEqual:
                TOKEN_DUMP("<=");
                break;
            case TokenType::Or:
                TOKEN_DUMP("|");
                break;
            case TokenType::LogicOr:
                TOKEN_DUMP("||");
                break;
            case TokenType::And:
                TOKEN_DUMP("&");
                break;
            case TokenType::LogicAnd:
                TOKEN_DUMP("&&");
                break;
            case TokenType::Mod:
                TOKEN_DUMP("%");
                break;
            case TokenType::At:
                TOKEN_DUMP("@");
                break;
            case TokenType::Colon:
                TOKEN_DUMP(":");
                break;
            case TokenType::KeywordNew:
                TOKEN_DUMP("new");
                break;
            default:
                TOKEN_DUMP("<unknown>");
        }
        os << name ;
#undef TOKEN_DUMP

    }

    // 读取一个Token
    void Lexer::GetNextToken() {
        if (mEof)
            return;
        // 跳过空格
        SkipSpace();
        switch (CURRENT_CHAR) {
            case _HM_C('\n'):
                TOKEN(TokenType::Delimiter, 1);
                NEXT_LINE();
                break;
            case _HM_C(';'):
                TOKEN(TokenType::Delimiter, 1);
                break;
            case _HM_C('('):
                TOKEN(TokenType::LeftParen, 1);
                break;
            case _HM_C(')'):
                TOKEN(TokenType::RightParen, 1);
                break;
            case _HM_C('['):
                TOKEN(TokenType::LeftBracket, 1);
                break;
            case _HM_C(']'):
                TOKEN(TokenType::RightBracket, 1);
                break;
            case _HM_C('{'):
                TOKEN(TokenType::LeftBrace, 1);
                break;
            case _HM_C('}'):
                TOKEN(TokenType::RightBrace, 1);
                break;
            case _HM_C('.'):
                TOKEN(TokenType::Dot, 1);
                break;
            case _HM_C('@'):
                TOKEN(TokenType::At, 1);
                break;
            case _HM_C(':'):
                TOKEN(TokenType::Colon, 1);
                break;
            case _HM_C('!'):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::NotEqual, 2);  // != not equal 不相等
                    NEXT();
                } else {
                    TOKEN(TokenType::Not, 1);  // !
                }
                break;
            case _HM_C('|'):
                if (NEXT_CHAR == _HM_C('|')) {
                    TOKEN(TokenType::LogicOr, 2);  // ||
                    NEXT();
                } else {
                    TOKEN(TokenType::Or, 1);  // |
                }
                break;
            case _HM_C('&'):
                if (NEXT_CHAR == _HM_C('&')) {
                    TOKEN(TokenType::LogicAnd, 2);  // &&
                    NEXT();
                } else {
                    TOKEN(TokenType::And, 1);  // &
                }
                break;
            case _HM_C('+'):
                if (NEXT_CHAR == _HM_C('+')) {
                    TOKEN(TokenType::Increase, 2); // ++
                    NEXT();
                } else if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::AddAssign, 2); // +=
                    NEXT();
                } else {
                    TOKEN(TokenType::Add, 1); // +
                }
                break;
            case _HM_C('>'):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::GreaterEqual, 2); // >=
                    NEXT();
                } else {
                    TOKEN(TokenType::Greater, 1); // +
                }
                break;
            case _HM_C('<'):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::LessEqual, 2); // >=
                    NEXT();
                } else {
                    TOKEN(TokenType::Less, 1); // +
                }
                break;
            case _HM_C('='):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::Equal, 2); // ==
                    NEXT();
                } else {
                    TOKEN(TokenType::Assign, 1); // =
                }
                break;
            case _HM_C('%'):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::ModAssign, 2); // %=
                    NEXT();
                } else {
                    TOKEN(TokenType::Mod, 1); // %
                }
                break;
            case _HM_C('-'):
                if (NEXT_CHAR == _HM_C('-')) { // --
                    TOKEN(TokenType::Decrease, 2); // 自减
                    NEXT();
                } else if (NEXT_CHAR == _HM_C('=')) { // -=
                    TOKEN(TokenType::SubAssign, 2);
                    NEXT();
                } else if (NEXT_CHAR == _HM_C('>')) { // -> Arrow 箭头
                    TOKEN(TokenType::Arrow, 2);
                    NEXT();
                } else {
                    TOKEN(TokenType::Sub, 1); // -
                }
                break;
            case _HM_C('*'):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::MulAssign, 2); // *=
                    NEXT();
                } else {
                    TOKEN(TokenType::Mul, 1);
                }
                break;
            case _HM_C(','):
                TOKEN(TokenType::Comma, 1);
                break;
            // 可能为注释 或者除号
            case _HM_C('/'):
                if (NEXT_CHAR == _HM_C('/') || NEXT_CHAR == _HM_C('*')) {
                    SkipComment();
                    return GetNextToken(); // 跳过之后继续读取一个token
                } else if (NEXT_CHAR == _HM_C('=')) { // /=
                    TOKEN(TokenType::DivAssign, 2);
                    NEXT();
                } else {
                    TOKEN(TokenType::Div, 1);
                }
                break;
            case _HM_C('\0'):
                mEof = true;
                TOKEN(TokenType::End, 0);
                return;
            default:
                if (IsCodeChar(CURRENT_CHAR)) {
                    ParseIdentifier();
                } else if (IsNumber(CURRENT_CHAR)) {
                    ParseNumber();
                } else if (CURRENT_CHAR == _HM_C('"') || CURRENT_CHAR == _HM_C('\'')) {
                    ParseString();
                } else {
                    LEXER_UNKOWNCHAR(CURRENT_CHAR);
                    NEXT();
                }
                return; // 已经到文件尾部了 返回就ok
        }
        NEXT();
    }

    void Lexer::SkipComment() {
        // TODO 仅支持单行 之后添加多行注释 /*   */
        do {
            NEXT();
        } while (CURRENT_CHAR != _HM_C('\n') || !mEof);
        NEXT_LINE();
        NEXT();
    }

    void Lexer::SkipSpace() {
        if (IsSpace(CURRENT_CHAR)){
            do {
                NEXT();
            } while (IsSpace(CURRENT_CHAR));
        }
    }

    bool Lexer::Match(TokenType type) {
        TokenType tok = PeekTokenType();
        if (tok == type) {
            mTokens.pop_front();
            return true;
        }
        return false;
    }

    Token Lexer::Peek(HMInteger i) {
        while (mTokens.size() < i) {
            if (mEof)
                return CURRENT_TOKEN;
            GetNextToken();
            mTokens.push_back(CURRENT_TOKEN);
        }
        return mTokens[i - 1];
    }

    void Lexer::ParseNumber() {
        // TODO 浮点数的支持
        TOKEN(TokenType::Number, 0);
        do {
            NEXT();
        } while (IsNumber(CURRENT_CHAR) || CURRENT_CHAR == _HM_C('.'));
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
        mCurrentToken.value.type = ValueType::Integer;
        mCurrentToken.value.intval = (int) hm_strtoi(mCurrentToken.start);
    }

    void Lexer::ParseString() {
        Buffer<HMChar> strbuf;
        HMChar first = CURRENT_CHAR;
        NEXT(); // 跳过 '   "  文本符
        TOKEN(TokenType::String, 0);

#define APPEND_CHAR(c) strbuf.push(&mVM->mGCHeap, c)
        do {
            if (CURRENT_CHAR == _HM_C('\\')) { // 跳过转义符
                NEXT();
                switch (CURRENT_CHAR) {
                    case _HM_C('n'):
                        APPEND_CHAR(_HM_C('\n'));
                        break;
                    case _HM_C('\''):
                        APPEND_CHAR(_HM_C('\''));
                        break;
                    case _HM_C('\"'):
                        APPEND_CHAR(_HM_C('"'));
                        break;
                    case _HM_C('\\'):
                        APPEND_CHAR(_HM_C('\\'));
                        break;
                    case _HM_C('a'):
                        APPEND_CHAR(_HM_C('\a'));
                        break;
                    case _HM_C('b'):
                        APPEND_CHAR(_HM_C('\b'));
                        break;
                    case _HM_C('f'):
                        APPEND_CHAR(_HM_C('\f'));
                        break;
                    case _HM_C('r'):
                        APPEND_CHAR(_HM_C('\r'));
                        break;
                    default:
                        // 不存在
                        break;
                }
            } else {
                APPEND_CHAR(CURRENT_CHAR);
            }
            NEXT();
        } while (CURRENT_CHAR != first);
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
        // 当前字符为 " ' 跳过   获取下一个字符
        NEXT();
        auto *objString = mVM->NewObject<HMString>(strbuf.data, strbuf.count);
        TOKEN_VALUE.type = ValueType::Object;
        TOKEN_VALUE.objval = objString;
        strbuf.clear(&mVM->mGCHeap);
    }

    void Lexer::ParseIdentifier() {
        TOKEN(TokenType::Identifier, 0);
        do {
            NEXT();
        } while (IsCodeChar(CURRENT_CHAR) || IsNumber(CURRENT_CHAR));
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
        HMChar identifierBuffer[MAX_IDENTIFIER_LENTH] = {_HM_C('\0')};
        hm_memcpy(identifierBuffer, mCurrentToken.start, mCurrentToken.length);
        TokenType tok = HMKeywords[identifierBuffer];
        if (tok != TokenType::End) {
            TOKEN_TYPE(tok);
        }

    }

    Token Lexer::Read() {
        if (!mTokens.empty()) {
            Token token = mTokens.front();
            mTokens.pop_front();
            return token;
        }
        GetNextToken();
        return CURRENT_TOKEN;
    }

    TokenType Lexer::ReadTokenType() {
        GetNextToken();
        return CURRENT_TOKEN.type;
    }

    TokenType Lexer::PeekTokenType() {
        if (mTokens.empty()) {
            GetNextToken();
            mTokens.push_back(mCurrentToken);
        }
        return mTokens[0].type;
    }

    void Lexer::Consume() {
        if (!mTokens.empty())
            mTokens.pop_front();
        else
            GetNextToken();
    }

    void Lexer::Consume(TokenType type) {
        if (PeekTokenType() == type)
            Consume();
        else {
            hm_cout << _HM_C(" missing token :  ");
            DumpTokenType(hm_cout, type);
            hm_cout << std::endl;
        }
    }

    Token::Token(TokenType mType, const HMChar *mStart, HMUINT32 mLength, HMUINT32 mLine) : type(mType),
                                                                                            start(mStart),
                                                                                            length(mLength),
                                                                                            line(mLine) {

    }

}