//
// Created by 曹顺 on 2019/2/11.
//

#include <iostream>
#include "lexer.h"
#include "obj/string.h"
namespace hypermind {
    inline bool IsSpace(HMChar ch){
        return ch == _HM_C(' ') || ch == _HM_C('\r');
    }
    inline bool IsNumber(HMChar ch) {
        return ch >= _HM_C('0') && ch <= _HM_C('9');
    }
    inline bool IsAlpha(HMChar ch) {
        return (ch >= _HM_C('a') && ch <= _HM_C('z')) || (ch >= _HM_C('A') && ch <= _HM_C('Z'));
    }
    inline bool IsCodeChar(HMChar ch) {
        return IsAlpha(ch) || ch == _HM_C('_');
    }

    void DumpToken(Token token) {
        std::string name;
        switch (token.mType) {
            case TokenType::End:
                name = "<结束>";
                break;
            case TokenType::Delimiter:
                name = "<行结束>";
                break;
            case TokenType::Add:
                name = "<加号 +>";
                break;
            case TokenType::Sub:
                name = "<减号 ->";
                break;
            case TokenType::Mul:
                name = "<乘号 *>";
                break;
            case TokenType::Div:
                name = "<除号 />";
                break;
            case TokenType::Identifier:
                name = "<标识符>";
                break;
            case TokenType::Number:
                name = "<数字>";
                break;
            default:
                name = "<未知>";
        }
        if (token.mLength != 1){
            std::string str(token.mStart, token.mLength);
            name += str;
        }

        std::cout << name  << " length:" << token.mLength << "  lineNum: " << token.mLine << std::endl;

    }

    //读取一个Token
    Token Lexer::ReadAToken() {
        if (mEof)
            return CURRENT_TOKEN;
        //跳过空格
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
                    return ReadAToken(); // 跳过之后继续读取一个token
                } else {
                    TOKEN(TokenType::Div, CURRENT_POS, 1, CURRENT_LINE);
                }
                break;
            case _HM_C('\0'):
                mEof = true;
                TOKEN(TokenType::Delimiter, CURRENT_POS, 0, CURRENT_LINE);
                return CURRENT_TOKEN; // 已经到文件尾部了 返回就ok
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
                // 不NEXT 直接返回
                return CURRENT_TOKEN;
        }
        NEXT();
        return CURRENT_TOKEN;
    }


    inline void Lexer::SkipComment() {
        // TODO 多行注释跳过 /*   */
        do {
            NEXT();
        } while (CURRENT_CHAR == _HM_C('\n'));
        CURRENT_LINE++;
    }

    inline void Lexer::SkipSpace() {
        if (IsSpace(CURRENT_CHAR)){
            do {
                NEXT();
            } while (IsSpace(CURRENT_CHAR));
        }
    }

    bool Lexer::Match(TokenType tokenKind) {
        Token token = Peek(1);
        if (token.mType == tokenKind) {
            mTokens.pop_front();
            return true;
        }
        return false;
    }

    Token Lexer::Peek(HMInteger i) {
        while (mTokens.size() < i) {
            if (mEof)
                return CURRENT_TOKEN;
            mTokens.push_back(ReadAToken());
        }
        return mTokens[i - 1];
    }

    inline void Lexer::ParseNumber() {
        TOKEN(TokenType::Number, CURRENT_POS, 0, CURRENT_LINE);
        do {
            NEXT();
        } while (IsNumber(CURRENT_CHAR) || CURRENT_CHAR == _HM_C('.'));
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
    }
    inline void Lexer::ParseString() {
        Buffer<HMChar> strbuf(mVM);
        HMChar first = CURRENT_CHAR;
        NEXT(); // 跳过 '   "  文本符
        TOKEN(TokenType::String, CURRENT_POS, 0, CURRENT_LINE);
        do {
            if (CURRENT_CHAR == _HM_C('\\')) { // 跳过转义符
                NEXT();
                switch (CURRENT_CHAR) {
                    case _HM_C('n'):
                        strbuf.Append('\n');
                        break;
                    case _HM_C('\''):
                        strbuf.Append('\'');
                        break;
                    case _HM_C('\"'):
                        strbuf.Append('"');
                        break;
                    case _HM_C('\\'):
                        strbuf.Append('\\');
                        break;
                    case _HM_C('a'):
                        strbuf.Append('\a');
                        break;
                    case _HM_C('b'):
                        strbuf.Append('\b');
                        break;
                    case _HM_C('f'):
                        strbuf.Append('\f');
                        break;
                    case _HM_C('r'):
                        strbuf.Append('\r');
                        break;
                    default:
                        // 不存在
                        break;
                }
            } else {
                strbuf.Append(CURRENT_CHAR);
            }
            NEXT();
        } while (CURRENT_CHAR != _HM_C(first));
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
        // 当前字符为 " ' 跳过   获取下一个字符
        NEXT();
        auto *objString = mVM->Allocate<HMString>();
        // 此时的HMString 会储存在Token.mValue 中函数编译完成后 会储存到constants 中
        // 当函数闭包没有引用时会释放constants的对象
        new(objString) HMString(mVM, strbuf.GetData(), strbuf.GetSize());
        TOKEN_VALUE.type = ValueType::Object;
        TOKEN_VALUE.objval = objString;
        strbuf.Clear();
    }
    inline void Lexer::ParseIdentifier() {
        TOKEN(TokenType::Identifier, CURRENT_POS, 0, CURRENT_LINE);
        do {
            NEXT();
        } while (IsCodeChar(CURRENT_CHAR) || IsNumber(CURRENT_CHAR));
        TOKEN_LENGTH((HMUINT32)(CURRENT_POS - TOKEN_START));
    }

    Lexer::~Lexer() {
    }

    Token Lexer::Read() {
        if (!mTokens.empty()) {
            Token token = mTokens.front();
            mTokens.pop_front();
            return token;
        }
        return ReadAToken();
    }

    Lexer::Lexer(VM *mVM, HMChar *mSource) : mVM(mVM), mSource(mSource) {}

    Token::Token(TokenType mType, HMChar *mStart, HMUINT32 mLength, HMUINT32 mLine) : mType(mType), mStart(mStart),
                                                                                mLength(mLength), mLine(mLine) {}
    Token::Token() {}


}