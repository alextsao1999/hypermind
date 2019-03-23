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
        return IsAlpha(ch) || ch == _HM_C('_') || (ch >= _HM_C('\u4e00') && ch <= _HM_C('\u9fa5'));
    }

    void Token::dump(Ostream &os) const {
        String name;
        switch (mType) {
            case TokenType::End:
                name = _HM_C("<结束>");
                break;
            case TokenType::Delimiter:
                name = _HM_C("<行结束>");
                break;
            case TokenType::Add:
                name = _HM_C("+");
                break;
            case TokenType::Sub:
                name = _HM_C("-");
                break;
            case TokenType::Mul:
                name = _HM_C("*");
                break;
            case TokenType::Div:
                name = _HM_C("/");
                break;
            case TokenType::Identifier:
            case TokenType::Number:
                name = String(mStart, mLength);
                break;
            case TokenType::String:
                break;
            case TokenType::Dot:
                name = _HM_C(".");
                break;
            case TokenType::Comma:
                name = _HM_C(",");
                break;
            case TokenType::LeftParen:
                name = _HM_C("(");
                break;
            case TokenType::RightParen:
                name = _HM_C(")");
                break;
            case TokenType::LeftBracket:
                name = _HM_C("[");
                break;
            case TokenType::RightBracket:
                name = _HM_C("]");
                break;
            case TokenType::LeftBrace:
                name = _HM_C("{");
                break;
            case TokenType::RightBrace:
                name = _HM_C("}");
                break;
            case TokenType::Increase:
                name = _HM_C("++");
                break;
            case TokenType::Decrease:
                name = _HM_C("--");
                break;
            case TokenType::Assign:
                name = _HM_C("=");
                break;
            case TokenType::AddAssign:
                name = _HM_C("+=");
                break;
            case TokenType::SubAssign:
                name = _HM_C("-=");
                break;
            case TokenType::MulAssign:
                name = _HM_C("*=");
                break;
            case TokenType::DivAssign:
                name = _HM_C("/=");
                break;
            case TokenType::ModAssign:
                name = _HM_C("%=");
                break;
            case TokenType::AndAssign:
                name = _HM_C("&=");
                break;
            case TokenType::OrAssign:
                name = _HM_C("|=");
                break;
            case TokenType::XorAssign:
                name = _HM_C("^=");
                break;
            case TokenType::Arrow:
                name = _HM_C("->");
                break;
            case TokenType::Not:
                name = _HM_C("!");
                break;
            case TokenType::Equal:
                name = _HM_C("==");
                break;
            case TokenType::NotEqual:
                name = _HM_C("!=");
                break;
            case TokenType::Greater:
                name = _HM_C(">");
                break;
            case TokenType::Less:
                name = _HM_C("<");
                break;
            case TokenType::GreaterEqual:
                name = _HM_C(">=");
                break;
            case TokenType::LessEqual:
                name = _HM_C("<=");
                break;
            case TokenType::Or:
                name = _HM_C("|");
                break;
            case TokenType::LogicOr:
                name = _HM_C("||");
                break;
            case TokenType::And:
                name = _HM_C("&");
                break;
            case TokenType::LogicAnd:
                name = _HM_C("&&");
                break;
            case TokenType::Mod:
                name = _HM_C("%");
                break;
            case TokenType::At:
                name = _HM_C("@");
                break;
            case TokenType::Colon:
                name = _HM_C(":");
                break;
            default:
                name = _HM_C("<未知>");
        }
        os << name ;
    }

    // 读取一个Token
    void Lexer::GetNextToken() {
        if (mEof)
            return;
        // 跳过空格
        SkipSpace();
        switch (CURRENT_CHAR) {
            case _HM_C('\n'):
                TOKEN(TokenType::Delimiter, CURRENT_POS, 1, CURRENT_LINE++);
                break;
            case _HM_C(';'):
                TOKEN(TokenType::Delimiter, CURRENT_POS, 1, CURRENT_LINE); 
                break;
            case _HM_C('('):
                TOKEN(TokenType::LeftParen, CURRENT_POS, 1, CURRENT_LINE);
                break;
            case _HM_C(')'):
                TOKEN(TokenType::RightParen, CURRENT_POS, 1, CURRENT_LINE);
                break;
            case _HM_C('['):
                TOKEN(TokenType::LeftBracket, CURRENT_POS, 1, CURRENT_LINE);
                break;
            case _HM_C(']'):
                TOKEN(TokenType::RightBracket, CURRENT_POS, 1, CURRENT_LINE);
                break;
            case _HM_C('{'):
                TOKEN(TokenType::LeftBrace, CURRENT_POS, 1, CURRENT_LINE);
                break;
            case _HM_C('}'):
                TOKEN(TokenType::RightBrace, CURRENT_POS, 1, CURRENT_LINE);
                break;
            case _HM_C('.'):
                TOKEN(TokenType::Dot, CURRENT_POS, 1, CURRENT_LINE);
                break;
            case _HM_C('!'):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::NotEqual, CURRENT_POS, 2, CURRENT_LINE);  // != not equal 不相等
                    NEXT();
                } else {
                    TOKEN(TokenType::Not, CURRENT_POS, 1, CURRENT_LINE);  // !
                }
                break;
            case _HM_C('+'):
                if (NEXT_CHAR == _HM_C('+')) {
                    TOKEN(TokenType::Increase, CURRENT_POS, 2, CURRENT_LINE); // ++
                    NEXT();
                } else if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::AddAssign, CURRENT_POS, 2, CURRENT_LINE); // +=
                    NEXT();
                } else {
                    TOKEN(TokenType::Add, CURRENT_POS, 1, CURRENT_LINE); // +
                }
                break;
            case _HM_C('='):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TokenType::Equal, CURRENT_POS, 2, CURRENT_LINE); // ==
                    NEXT();
                } else {
                    TOKEN(TokenType::Assign, CURRENT_POS, 1, CURRENT_LINE); // =
                }
                break;
            case _HM_C('-'):
                if (NEXT_CHAR == _HM_C('-')) { // --
                    TOKEN(TokenType::Decrease, CURRENT_POS, 1, CURRENT_LINE); // 自减
                    NEXT();
                } else if (NEXT_CHAR == _HM_C('=')) { // -=
                    TOKEN(TokenType::SubAssign, CURRENT_POS, 1, CURRENT_LINE);
                    NEXT();
                } else if (NEXT_CHAR == _HM_C('>')) { // -> Arrow 箭头
                    TOKEN(TokenType::Arrow, CURRENT_POS, 1, CURRENT_LINE);
                    NEXT();
                } else {
                    TOKEN(TokenType::Sub, CURRENT_POS, 1, CURRENT_LINE); // -
                }
                break;
            case _HM_C('*'):
                TOKEN(TokenType::Mul, CURRENT_POS, 1, CURRENT_LINE);
                break;
            case _HM_C(','):
                TOKEN(TokenType::Comma, CURRENT_POS, 1, CURRENT_LINE);
                break;
            // 可能为注释 或者除号
            case _HM_C('/'):
                if (NEXT_CHAR == _HM_C('/') || NEXT_CHAR == _HM_C('*')) {
                    SkipComment();
                    return GetNextToken(); // 跳过之后继续读取一个token
                } else {
                    TOKEN(TokenType::Div, CURRENT_POS, 1, CURRENT_LINE);
                }
                break;
            case _HM_C('\0'):
                mEof = true;
                TOKEN(TokenType::End, CURRENT_POS, 0, CURRENT_LINE);
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
        // TODO 多行注释跳过 /*   */
        do {
            NEXT();
        } while (CURRENT_CHAR == _HM_C('\n'));
        CURRENT_LINE++;
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
        TOKEN(TokenType::Number, CURRENT_POS, 0, CURRENT_LINE);
        do {
            NEXT();
        } while (IsNumber(CURRENT_CHAR) || CURRENT_CHAR == _HM_C('.'));
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
        mCurrentToken.mValue.type = ValueType::Integer;
        mCurrentToken.mValue.intval = (int) hm_strtoi(mCurrentToken.mStart);
    }
    void Lexer::ParseString() {
        Buffer<HMChar> strbuf(mVM);
        HMChar first = CURRENT_CHAR;
        NEXT(); // 跳过 '   "  文本符
        TOKEN(TokenType::String, CURRENT_POS, 0, CURRENT_LINE);
        do {
            if (CURRENT_CHAR == _HM_C('\\')) { // 跳过转义符
                NEXT();
                switch (CURRENT_CHAR) {
                    case _HM_C('n'):
                        strbuf.Append(_HM_C('\n'));
                        break;
                    case _HM_C('\''):
                        strbuf.Append(_HM_C('\''));
                        break;
                    case _HM_C('\"'):
                        strbuf.Append(_HM_C('"'));
                        break;
                    case _HM_C('\\'):
                        strbuf.Append(_HM_C('\\'));
                        break;
                    case _HM_C('a'):
                        strbuf.Append(_HM_C('\a'));
                        break;
                    case _HM_C('b'):
                        strbuf.Append(_HM_C('\b'));
                        break;
                    case _HM_C('f'):
                        strbuf.Append(_HM_C('\f'));
                        break;
                    case _HM_C('r'):
                        strbuf.Append(_HM_C('\r'));
                        break;
                    default:
                        // 不存在
                        break;
                }
            } else {
                strbuf.Append(CURRENT_CHAR);
            }
            NEXT();
        } while (CURRENT_CHAR != first);
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
        // 当前字符为 " ' 跳过   获取下一个字符
        NEXT();
        auto *objString = mVM->NewObject<HMString>(strbuf.GetBuffer(), strbuf.GetCount());
        TOKEN_VALUE.type = ValueType::Object;
        TOKEN_VALUE.objval = objString;
        strbuf.Clear();
    }

    void Lexer::ParseIdentifier() {
        TOKEN(TokenType::Identifier, CURRENT_POS, 0, CURRENT_LINE);
        do {
            NEXT();
        } while (IsCodeChar(CURRENT_CHAR) || IsNumber(CURRENT_CHAR));
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
        HMChar identifierBuffer[MAX_IDENTIFIER_LENTH] = {_HM_C('\0')};
        hm_memcpy(identifierBuffer, mCurrentToken.mStart, mCurrentToken.mLength);
        TokenType tok = HMKeywords[identifierBuffer];
        if (tok != TokenType::End) {
            TOKEN_TYPE(tok);
        }

    }

    Lexer::~Lexer() = default;

    Token Lexer::Read() {
        if (!mTokens.empty()) {
            Token token = mTokens.front();
            mTokens.pop_front();
            return token;
        }
        GetNextToken();
        return CURRENT_TOKEN;
    }

    Lexer::Lexer(VM *mVM, HMChar *mSource) : mVM(mVM), mSource(mSource) {

    }

    TokenType Lexer::ReadTokenType() {
        GetNextToken();
        return CURRENT_TOKEN.mType;
    }

    TokenType Lexer::PeekTokenType() {
        if (mTokens.empty()) {
            GetNextToken();
            mTokens.push_back(mCurrentToken);
        }
        return mTokens[0].mType;
    }

    void Lexer::Consume() {
        if (!mTokens.empty())
            mTokens.pop_front();
        else
            GetNextToken();
    }

    Token::Token(TokenType mType, const HMChar *mStart, HMUINT32 mLength, HMUINT32 mLine) : mType(mType),
                                                                                            mStart(mStart),
                                                                                            mLength(mLength),
                                                                                            mLine(mLine) {

    }

}