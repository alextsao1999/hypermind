//
// Created by 曹顺 on 2019/2/11.
//

#ifndef HYPERMIND_PARSER_H
#define HYPERMIND_PARSER_H

#include <lexer.h>

namespace hypermind {
    class Parser;
    enum class SignatureType {
        Constructor,
        Method,
        Getter,
        Setter,
        Iterator,
        Subscript,
        SubscriptSetter
    };   //方法的签名

    // 签名构造
    struct Signature {
        SignatureType type;  // 签名类型
        const HMChar *name;    // 签名
        HMUINT32 length;    // 签名长度
        HMUINT32 argNum;    // 参数个数
    };

    // 绑定能力
    enum class BindPower {
        None,
        Lowest,    //最低绑定能力
        Assign,    // =
        Condition,   // ? :
        LogicOr,    // ||
        LogicAnd,   // &&
        Equal,      // == !=
        Is,        // is
        Cmp,       // < > <= >=
        BitOr,    // |
        BitAnd,   // &
        BitShift, // << >>
        Range,       // ..
        Term,      // + -
        Factor,      // * / %
        Unary,    // - ! ~
        Call,     // . () []
        Highest
    } BindPower;   // 定义操作符绑定权值(优先级)
    typedef void (Parser::*DenotationFn)(bool canAssign);
    typedef void (Parser::*MethodSignatureFn)(Signature *signature);

    struct SymbolBindRule {
        const HMChar *id;          // 符号
        // 左绑定权值,不关注左边操作数的符号此值为0
        DenotationFn nud;
        DenotationFn led;
        MethodSignatureFn methodSign;

        SymbolBindRule(const HMChar *id, DenotationFn nud, DenotationFn led,
                       MethodSignatureFn methodSign);

    };

    class Parser {
    protected:
        const HMChar *mFileName{};
        const Lexer &mLexer;

    public:
        Parser(const HMChar *mFileName, const Lexer &mLexer) : mFileName(mFileName), mLexer(mLexer) {}
        void BindFunction(bool canAssign) {


        };

        void Signature(Signature sign){

        };

    };
    void test(){
        auto t = &Parser::BindFunction;

    }

}


#endif //HYPERMIND_PARSER_H
