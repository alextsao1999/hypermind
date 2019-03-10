//
// Created by 曹顺 on 2019/2/11.
//

#ifndef HYPERMIND_PARSER_H
#define HYPERMIND_PARSER_H
#define UNPREC {-1, false}
#define PREC(p,a) {p, a}

#include <memory>
#include "lexer.h"
#include "ast.h"
#define make_ptr(v) std::make_shared<v>()

namespace hypermind {
    struct Precedence {
        HMInteger prec;
        HMBool association; // true 为左结合  false 为右结合
    };
    static Precedence opPrecs[] = {
            /*End,*/   UNPREC,
            /*Delimiter,*/ UNPREC,
            /*Number*/  UNPREC,
            /*String*/  UNPREC,
            /*Identifier*/  UNPREC,
            /*Dot*/  UNPREC,
            /*Comma*/  UNPREC,
            /*LeftParen*/  UNPREC,
            /*RightParen*/  UNPREC,
            /*LeftBracket*/  UNPREC,
            /*RightBracket*/  UNPREC,
            /*LeftBrace*/  UNPREC,
            /*RightBrace*/  UNPREC,

            //四则运算 + - * /
            /*Add*/  PREC(11, true),
            /*Sub*/  PREC(11, true),
            /*Mul*/  PREC(12, true),
            /*Div*/  PREC(12, true),

            /*Increase*/  UNPREC,
            /*Decrease*/  UNPREC, // --
            /*Assign*/  PREC(1, false), // assignment 赋值 =
            /*AddAssign*/  PREC(1, false), // += Addition assignment
            /*SubAssign*/  PREC(1, false), // -=Subtraction assignment and so on.....
            /*MulAssign*/  PREC(1, false),
            /*DivAssign*/  PREC(1, false),
            /*ModAssign*/  PREC(1, false),
            /*AndAssign*/  PREC(1, false),
            /*OrAssign*/  PREC(1, false),
            /*XorAssign*/  PREC(1, false),
            //等式 不等式
            /*Arrow*/  UNPREC, //  -> 箭头
            // 后面这些TOKEN 暂时用不到 之后再添加吧
            /*Not*/  UNPREC, // !
            /*Equal*/  PREC(8, true),  // ==
            /*NotEqual*/  PREC(8, true), // !=
            /*Greater*/  PREC(9, true), // >
            /*Less*/  PREC(9, true), // <
            /*GreaterEqual*/  PREC(9, true), // >=
            /*LessEqual*/  PREC(9, true), // <=
            // | 或
            /*Or*/  PREC(7, true),
            /*LogicOr*/  PREC(3, false),
            // & 且
            /*And*/  PREC(7, true),
            /*LogicAnd*/  PREC(4, false),
            // #
            /*Mod*/  PREC(12, true), // % remainder 取余
            /*At*/  UNPREC,  // @
            /*Colon*/  UNPREC, //冒号 :
           /*KeywordIf*/ UNPREC,
           /*KeywordElse*/ UNPREC,
           /*KeywordWhile*/ UNPREC,
           /*KeywordBreak*/ UNPREC,
           /*KeywordContinue*/ UNPREC,
           /*KeywordFor*/ UNPREC,
           /*keywordClass*/ UNPREC,
           /*KeywordFunction*/ UNPREC,
           /*KeywordNew*/ UNPREC,
           /*KeywordReturn*/ UNPREC,
           /*KeywordVar*/ UNPREC,
           /*KeywordThis*/ UNPREC,
           /*KeywordSuper*/ UNPREC,
           /*KeywordTrue*/ UNPREC,
           /*KeywordFalse*/ UNPREC,
           /*KeywordStatic*/ UNPREC,
    };
    class Parser {
    protected:
        const HMChar *mFileName{};
        Lexer &mLexer;
        inline ASTExprPtr ParsePrimary();
        inline ASTExprPtr ParseUnary();
        inline ASTExprPtr ParseBinaryOp(ASTExprPtr lhs, HMInteger prec);

    public:
        Parser(const HMChar *mFileName, Lexer &mLexer) : mFileName(mFileName), mLexer(mLexer) {}

        ASTExprPtr ParseExpression();
        inline ASTStmtPtr ParseVarStmt();
        inline ASTStmtPtr ParseFunctionStmt();
        inline ASTStmtPtr ParseIfStmt();
        inline ASTStmtPtr ParseWhileStmt();
        inline ASTStmtPtr ParseBreakStmt();
        inline ASTStmtPtr ParseContinueStmt();
        inline ASTStmtPtr ParseReturnStmt();
        inline ASTStmtPtr ParseClassStmt();
        inline ASTBlockPtr ParseBlock();


    };

}


#endif //HYPERMIND_PARSER_H
