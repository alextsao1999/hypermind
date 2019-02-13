//
// Created by 曹顺 on 2019/2/11.
//

#include <iostream>
#include "lexer.h"

namespace hypermind {
    inline bool isSpace(HMChar ch){
        return ch == _HM_C(' ') || ch == _HM_C('\r');
    }
    inline bool isNumber(HMChar ch) {
        return ch >= _HM_C('0') && ch <= _HM_C('9');
    }
    inline bool isAlpha(HMChar ch) {
        return (ch >= _HM_C('a') && ch <= _HM_C('z')) || (ch >= _HM_C('A') && ch <= _HM_C('Z'));
    }
    inline bool isCodeChar(HMChar ch) {
        return isAlpha(ch) || ch == _HM_C('_');
    }

    void dumpToken(Token token) {
        std::string name;
        switch (token.mType) {
            case TK_EOF:
                name = "<结束>";
                break;
            case TK_EOL:
                name = "<行结束>";
                break;
            case TK_ADD:
                name = "<加号 +>";
                break;
            case TK_SUB:
                name = "<减号 ->";
                break;
            case TK_MUL:
                name = "<乘号 *>";
                break;
            case TK_DIV:
                name = "<除号 />";
                break;
            case TK_IDENTIFIER:
                name = "<标识符>";
                break;
            case TK_NUMBER:
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
    Token Lexer::readAToken() {
        if (mEof)
            return CURRENT_TOKEN;
        //跳过空格
        skipSpace();
        switch (CURRENT_CHAR) {
            case _HM_C('\n'):
                TOKEN(TK_EOL, NOW, 1, CURRENT_LINE++);
                break;
            case _HM_C(';'):
                TOKEN(TK_EOL, NOW, 1, CURRENT_LINE); // 同样也是行分隔符 但是不会增加行号
                break;
            case _HM_C('('):
                TOKEN(TK_LPAREN, NOW, 1, CURRENT_LINE);
                break;
            case _HM_C(')'):
                TOKEN(TK_RPAREN, NOW, 1, CURRENT_LINE);
                break;
            case _HM_C('['):
                TOKEN(TK_LBRACKET, NOW, 1, CURRENT_LINE);
                break;
            case _HM_C(']'):
                TOKEN(TK_RBRACKET, NOW, 1, CURRENT_LINE);
                break;
            case _HM_C('{'):
                TOKEN(TK_LBRACE, NOW, 1, CURRENT_LINE);
                break;
            case _HM_C('}'):
                TOKEN(TK_RBRACE, NOW, 1, CURRENT_LINE);
                break;
            case _HM_C('.'):
                TOKEN(TK_DOT, NOW, 1, CURRENT_LINE);
                break;
            case _HM_C('!'):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TK_NE, NOW, 2, CURRENT_LINE);  // != not equal 不相等
                    NEXT();
                } else {
                    TOKEN(TK_NOT, NOW, 1, CURRENT_LINE);  // !
                }
                break;
            case _HM_C('+'):
                if (NEXT_CHAR == _HM_C('+')) {
                    TOKEN(TK_INC, NOW, 2, CURRENT_LINE); // ++
                    NEXT();
                } else if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TK_AA, NOW, 2, CURRENT_LINE); // +=
                    NEXT();
                } else {
                    TOKEN(TK_ADD, NOW, 1, CURRENT_LINE); // +
                }
                break;
            case _HM_C('='):
                if (NEXT_CHAR == _HM_C('=')) {
                    TOKEN(TK_EQ, NOW, 2, CURRENT_LINE); // ==
                    NEXT();
                } else {
                    TOKEN(TK_AS, NOW, 1, CURRENT_LINE); // =
                }
                break;
            case _HM_C('-'):
                if (NEXT_CHAR == _HM_C('-')) { // --
                    TOKEN(TK_DEC, NOW, 1, CURRENT_LINE); // 自减
                    NEXT();
                } else if (NEXT_CHAR == _HM_C('=')) { // -=
                    TOKEN(TK_SA, NOW, 1, CURRENT_LINE);
                    NEXT();
                } else {
                    TOKEN(TK_SUB, NOW, 1, CURRENT_LINE); // -
                }
                break;
            case _HM_C('*'):
                TOKEN(TK_MUL, NOW, 1, CURRENT_LINE);
                break;
            case _HM_C(','):
                TOKEN(TK_COMMA, NOW, 1, CURRENT_LINE);
                break;
            // 可能为注释 或者除号
            case _HM_C('/'):
                if (NEXT_CHAR == _HM_C('/') || NEXT_CHAR == _HM_C('*')) {
                    skipComment();
                    return readAToken(); // 跳过之后继续读取一个token
                } else {
                    TOKEN(TK_DIV, NOW, 1, CURRENT_LINE);
                }
                break;
            case _HM_C('\0'):
                mEof = true;
                TOKEN(TK_EOF, NOW, 0, CURRENT_LINE);
                return CURRENT_TOKEN; // 已经到文件尾部了 返回就ok
            default:
                if (isCodeChar(CURRENT_CHAR)) {
                    parseIdentifier();
                } else if (isNumber(CURRENT_CHAR)) {
                    parseNumber();
                } else if (CURRENT_CHAR == _HM_C('"') || CURRENT_CHAR == _HM_C('\'')) {
                    parseString();
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


    inline void Lexer::skipComment() {
        do {
            NEXT();
        } while (CURRENT_CHAR == _HM_C('\n'));
        CURRENT_LINE++;
    }

    inline void Lexer::skipSpace() {
        if (isSpace(CURRENT_CHAR)){
            do {
                NEXT();
            } while (isSpace(CURRENT_CHAR));
        }
    }

    bool Lexer::match(TokenType type) {
        Token token = peek(1);
        if (token.mType == type) {
            mTokens.pop_front();
            return true;
        }
        return false;
    }

    Token Lexer::peek(int i) {
        while (mTokens.size() < i) {
            if (mEof)
                return CURRENT_TOKEN;
            mTokens.push_back(readAToken());
        }
        return mTokens[i - 1];
    }

    inline void Lexer::parseNumber() {
        TOKEN(TK_NUMBER, NOW, 0, CURRENT_LINE);
        do {
            NEXT();
        } while (isNumber(CURRENT_CHAR) || CURRENT_CHAR == _HM_C('.'));
        TOKEN_LENGTH((uint32_t)(NOW - TOKEN_START));
    }
    inline void Lexer::parseString() {
        TOKEN(TK_STRING, NOW, 0, CURRENT_LINE);
        HMChar first = CURRENT_CHAR;
        do {
            if (CURRENT_CHAR == _HM_C('\\')) // 跳过转义符
                NEXT();
            NEXT();
        } while (CURRENT_CHAR != _HM_C(first));
        // 当前字符为 " ' 跳过 "   获取下一个字符
        NEXT();
        TOKEN_LENGTH((uint32_t)(NOW - TOKEN_START));
    }
    inline void Lexer::parseIdentifier() {
        TOKEN(TK_IDENTIFIER, NOW, 0, CURRENT_LINE);
        do {
            NEXT();
        } while (isCodeChar(CURRENT_CHAR) || isNumber(CURRENT_CHAR));
        TOKEN_LENGTH((uint32_t)(NOW - TOKEN_START));
    }

    Lexer::~Lexer() {
    }

    Token Lexer::read() {
        if (!mTokens.empty()) {
            Token token = mTokens.front();
            mTokens.pop_front();
            return token;
        }
        return readAToken();
    }

    Token::Token(TokenType mType, HMChar *mStart, int mLength, int mLine) : mType(mType), mStart(mStart),
                                                                                mLength(mLength), mLine(mLine) {}
    Token::Token() {}


}