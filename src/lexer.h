//
// Created by 曹顺 on 2019/2/11.
//

#ifndef HYPERMIND_LEXER_H
#define HYPERMIND_LEXER_H

#include <hypermind.h>
#include <deque>

#define NEXT_CHAR mSource[mPosition + 1]

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

//当前位置
#define NOW (mSource + mPosition)
//下一个位置
#define NEXT() {mPosition++;}

#define LEXER_UNKOWNCHAR(str) std::cout << "词法分析器错误  : 未知符号 -> " << str << "  行号 : " << mLine << std::endl

namespace hypermind {
    inline bool isNumber(HMChar ch);
    inline bool isAlpha(HMChar ch);
    inline bool isCodeChar(HMChar ch);
    inline bool isSpace(HMChar ch);

    typedef enum {
        TK_EOF,
        TK_EOL,
        TK_NUMBER, //数字
        TK_STRING, //文本
        TK_IDENTIFIER, //标识符
        TK_DOT, //点号
        TK_COMMA, //逗号
        TK_LPAREN, // ( 小括号
        TK_RPAREN,  // )
        TK_LBRACKET, // [ 中括号
        TK_RBRACKET, // ]
        TK_LBRACE,  // { 大括号
        TK_RBRACE, // }

        //四则运算 + - * /
        TK_ADD,
        TK_SUB,
        TK_MUL,
        TK_DIV,

        TK_INC,  // ++
        TK_DEC, // --
        TK_AA, // += Addition assignment
        TK_SA, // -=Subtraction assignment and so on.....
        //等式 不等式

        TK_AS, // assignment 赋值

        // 后面这些TOKEN 暂时用不到 之后再添加吧
        TK_NOT, // !
        TK_EQ,  // ==
        TK_NE, // !=
        TK_GT, // >
        TK_LT, // <
        TK_GE, // >=
        TK_LE, // <=

        // | 或
        TK_OR,
        TK_LOGIC_OR,

        // & 且
        TK_AND,
        TK_LOGIC_AND,

        // #
        TK_SHARP,  // #
        TK_DOLLAR, // $ 美元
        TK_MOD, // % remainder 取余

        TK_AT,  // @

        TK_COLON  //冒号 :

    } TokenType;

    class Token {
    public:
        TokenType mType;
        const HMChar *mStart;
        int mLength;
        int mLine;

        Token(TokenType mType, HMChar *mStart, int mLength, int mLine);
        Token();

    };

    void dumpToken(Token token);

    class Lexer {
    protected:
        bool mEof{};
        std::deque<Token> mTokens;

        Token readAToken();
        inline void skipSpace();
        inline void skipComment();
        inline void parseIdentifier();
        inline void parseString();
        inline void parseNumber();
        Token mCurrentToken;

    public:
        explicit Lexer(HMChar *str) : mSource(str) {};
        ~Lexer();

        Token read();
        Token peek(int i);
        bool match(TokenType tokenKind);
        HMChar *getSource() {
            return mSource;
        }

    private:
        HMChar *mSource;
        int mPosition{};
        int mLine{1};
    };
}

#endif //HYPERMIND_LEXER_H
