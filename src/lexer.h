//
// Created by 曹顺 on 2019/2/11.
//

#ifndef HYPERMIND_LEXER_H
#define HYPERMIND_LEXER_H

#include <hypermind.h>
#include <deque>
#include <vm.h>
#include <buffer.h>
#define NEXT_CHAR mSource[mPosition + 1]

//当前位置
#define CURRENT_POS (mSource + mPosition)
#define CURRENT_CHAR mSource[mPosition]
#define CURRENT_LINE mLine
#define CURRENT_TOKEN mCurrentToken

// 设置当前Token
#define TOKEN(k, s, len, line) { mCurrentToken.mType = k; \
mCurrentToken.mStart = s;  \
mCurrentToken.mLength = len; \
mCurrentToken.mLine = line; }

#define TOKEN_TYPE(k) mCurrentToken.mType = k;
#define TOKEN_LENGTH(l) {mCurrentToken.mLength = l;}
#define TOKEN_START mCurrentToken.mStart

//下一个位置
#define NEXT() {mPosition++;}

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

        Colon  //冒号 :
    };

    struct Token {
        TokenType mType;
        const HMChar *mStart;
        HMUINT32 mLength;
        HMUINT32 mLine;

        Token(TokenType mType, HMChar *mStart, HMUINT32 mLength, HMUINT32 mLine);
        Token();

    };

    void DumpToken(Token token);

    class Lexer {
    protected:
        bool mEof{false};
        std::deque<Token> mTokens;

        Token ReadAToken();
        inline void SkipSpace();
        inline void SkipComment();
        inline void ParseIdentifier();
        inline void ParseString();
        inline void ParseNumber();
        Token mCurrentToken;

    public:
        Lexer(VM *mVM, HMChar *mSource);

        ~Lexer();

        Token Read();
        Token Peek(HMInteger i);
        bool Match(TokenType tokenKind);
        HMChar *GetSource() {
            return mSource;
        }

    private:
        VM *mVM;
        HMChar *mSource;
        HMUINT32 mPosition{0};
        HMUINT32 mLine{1};
    };
}

#endif //HYPERMIND_LEXER_H
