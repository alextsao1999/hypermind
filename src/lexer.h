//
// Created by 曹顺 on 2019/2/11.
//

#ifndef HYPERMIND_LEXER_H
#define HYPERMIND_LEXER_H

#include <cstdlib>
#include <deque>
#include <map>
#include "hypermind.h"
#include "vm.h"
#include "buffer.h"
#include "obj/value.h"
#define NEXT_CHAR mSource[mPosition + 1]

//当前位置
#define CURRENT_POS &mSource[mPosition]
#define CURRENT_CHAR mSource[mPosition]
#define CURRENT_LINE mLine
#define CURRENT_TOKEN mCurrentToken

// 设置当前Token
#define TOKEN(K, len) { mCurrentToken.type = K; \
mCurrentToken.start = CURRENT_POS;  \
mCurrentToken.length = len; \
mCurrentToken.line = CURRENT_LINE; \
mCurrentToken.value = {ValueType::Undefined}; }

#define TOKEN_TYPE(K) mCurrentToken.type = K;
#define TOKEN_LENGTH(l) {mCurrentToken.length = l;}
#define TOKEN_START mCurrentToken.start
#define TOKEN_VALUE mCurrentToken.value

//下一个位置
#define NEXT() {mPosition++;}
#define NEXT_LINE() {CURRENT_LINE++; mLineStart = mPosition;}

#define LEXER_UNKOWNCHAR(str) std::cout << "词法分析器错误  : 未知字符 -> " << str << "  行号 : " << mLine << std::endl

namespace hypermind {
    inline bool IsNumber(HMChar ch);
    inline bool IsAlpha(HMChar ch);
    inline bool IsCodeChar(HMChar ch);
    inline bool IsSpace(HMChar ch);

    enum class TokenType {
        End,
        Delimiter,
        Number, //数字
        String, //文本
        Identifier, //标识符
        Dot, //点号
        Comma, //逗号
        LeftParen, // ( 小括号
        RightParen,  // )
        LeftBracket, // [ 中括号
        RightBracket, // ]
        LeftBrace,  // { 大括号
        RightBrace, // }

        //四则运算 + - * /
        Add,
        Sub,
        Mul,
        Div,

        Increase,  // ++
        Decrease, // --
        Assign, // assignment 赋值 =
        AddAssign, // += Addition assignment
        SubAssign, // -=Subtraction assignment and so on.....
        MulAssign,
        DivAssign,
        ModAssign,
        AndAssign,
        OrAssign,
        XorAssign,
        //等式 不等式
        Arrow, //  -> 箭头

        // 后面这些TOKEN 暂时用不到 之后再添加吧
        Not, // !
        Equal,  // ==
        NotEqual, // !=
        Greater, // >
        Less, // <
        GreaterEqual, // >=
        LessEqual, // <=

        // | 或
        Or,
        LogicOr,

        // & 且
        And,
        LogicAnd,

        // #
        Mod, // % remainder 取余

        At,  // @

        Colon,  //冒号 :

        KeywordIf,
        KeywordElse,
        KeywordWhile,
        KeywordBreak,
        KeywordContinue,
        KeywordFor,
        keywordClass,
        KeywordFunction,
        KeywordNew,
        KeywordReturn,
        KeywordVar,
        KeywordThis,
        KeywordSuper,
        KeywordTrue,
        KeywordFalse,
        KeywordStatic,
    };

    void DumpTokenType(Ostream &os, TokenType type);

    static std::map<String, TokenType> HMKeywords = {
            {_HM_C("if"),       TokenType::KeywordIf},
            {_HM_C("else"),     TokenType::KeywordElse},
            {_HM_C("while"),    TokenType::KeywordWhile},
            {_HM_C("break"),    TokenType::KeywordBreak},
            {_HM_C("continue"),    TokenType::KeywordContinue},
            {_HM_C("for"),      TokenType::KeywordFor},
            {_HM_C("class"),    TokenType::keywordClass},
            {_HM_C("function"), TokenType::KeywordFunction},
            {_HM_C("new"),      TokenType::KeywordNew},
            {_HM_C("return"),   TokenType::KeywordReturn},
            {_HM_C("var"),   TokenType::KeywordVar},
            {_HM_C("this"),   TokenType::KeywordThis},
            {_HM_C("super"),   TokenType::KeywordSuper},
            {_HM_C("true"),   TokenType::KeywordTrue},
            {_HM_C("false"),   TokenType::KeywordFalse},
            {_HM_C("static"),   TokenType::KeywordStatic},

    };

    struct Token {
        TokenType type{TokenType::End};
        const HMChar *start{nullptr};
        HMUINT32 length{0};
        HMUINT32 line{0};
        Value value{ValueType::Undefined};
        Token(TokenType mType, const HMChar *mStart, HMUINT32 mLength, HMUINT32 mLine);
        Token() = default;
        void dump(Ostream &os) const;

    };

    class Lexer {
    protected:
        std::deque<Token> mTokens;

        inline void GetNextToken();
        inline void SkipSpace();
        inline void SkipComment();
        inline void ParseIdentifier();
        inline void ParseString();
        inline void ParseNumber();

    public:
        HMBool mEof{false};
        Token mCurrentToken;

        Lexer(VM *mVM, HMChar *mSource) : mVM(mVM), mSource(mSource) {};
        ~Lexer() = default;

        Token Read();
        TokenType ReadTokenType();
        TokenType PeekTokenType();
        void Consume();
        void Consume(TokenType type);
        Token Peek(HMInteger i = 1);
        bool Match(TokenType tokenType);

        inline HMUINT32 GetCurrentLineNumber(){
            return CURRENT_LINE;
        };
        inline HMUINT32 GetLineColumn() {
            return mPosition - mLineStart;
        };

        HMChar *GetSource() {
            return mSource;
        }
    private:
        VM *mVM;
        HMChar *mSource;
        HMUINT32 mPosition{0};
        HMUINT32 mLine{1};
        HMUINT32 mLineStart{0};
    };
}

#endif //HYPERMIND_LEXER_H
