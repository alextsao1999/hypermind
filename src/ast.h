//
// Created by 曹顺 on 2019/3/1.
//
#ifndef HYPERMIND_AST_H
#define HYPERMIND_AST_H

#include "hypermind.h"
#include <memory>
#include "obj/value.h"
#include "lexer.h"

#define AST_NODE(n,p)  \
    struct n;  \
    using p = std::shared_ptr<n>;  \
    struct n : public ASTNode
#define AST_STMT(n, p) \
    struct n;  \
    using p = std::shared_ptr<n>;  \
    struct n : public ASTStmt
#define AST_EXPR(n, p) \
    struct n;  \
    using p = std::shared_ptr<n>;  \
    struct n : public ASTExpr
#define AST_DECL() void codegen(){};void dump(Ostream &os, int ind);
#define AST_DUMP(n) void n::dump(Ostream &os, int ind)
namespace hypermind {
    struct ASTNode {
        virtual void dump(Ostream &os, int ind){};
        virtual void codegen(){};
    };
    using ASTNodePtr = std::shared_ptr<ASTNode>;

    // 语句Node
    AST_NODE(ASTStmt, ASTStmtPtr) {

    };

    // 表达式Node
    AST_STMT(ASTExpr, ASTExprPtr) {

    };

    // 语法块Node
    AST_NODE(ASTBlock, ASTBlockPtr) {
        std::vector<ASTStmtPtr> stmts;
        inline void addStmt(ASTStmtPtr &&np) {
            stmts.push_back(np);
        }
        AST_DECL();
    };

    // 表达式中的变量
    AST_EXPR(ASTVariable, ASTVariablePtr){
        Token mVar;
        AST_DECL();
    };

    // 值的字面量
    AST_EXPR(ASTLiteral, ASTLiteralPtr){
        Value mValue{};
        AST_DECL();
    };

    // 二元表达式
    AST_EXPR(ASTBinary, ASTBinaryPtr) {
        Token mOp;
        ASTExprPtr mLHS; // 产生式左边
        ASTExprPtr mRHS; // 产生式右边
        //ASTBinary(const Token &op, const ASTExprPtr &mLHS, const ASTExprPtr &mRHS);
        AST_DECL();
    };

    // If 语句
    AST_STMT(ASTIfStmt, ASTIfStmtPtr) {
        ASTExprPtr mCondition; // 条件
        // Block or Expr
        ASTNodePtr mThen;
        ASTNodePtr mElse;
        AST_DECL();
    };

    // while 语句
    AST_STMT(ASTWhileStmt, ASTWhileStmtPtr) {
        ASTExprPtr mCondition;
        ASTNodePtr mBlock;
        AST_DECL();
    };

    // break 语句
    AST_STMT(ASTBreakStmt, ASTBreakStmtPtr) {
        AST_DECL();
    };

    // continue 语句
    AST_STMT(ASTContinueStmt, ASTContinueStmtPtr) {
        AST_DECL();
    };

    // 变量声明语句
    AST_STMT(ASTVarStmt, ASTVarStmtPtr) {
        Token mIdentifier;
        ASTExprPtr mValue; // 初始值表达式
        AST_DECL();
    };

    // AST列表
    AST_NODE(ASTList, ASTListPtr) {
        std::vector<ASTNodePtr> elements;
        AST_DECL();
    };

    // AST函数声明
    AST_STMT(ASTFunctionStmt, ASTFunctionStmtPtr) {
        Token mName; // 函数名称
        ASTNodePtr mParams; // 形式参数
        ASTBlockPtr mBody; // 函数体
        AST_DECL();
    };

    // AST类声明
    AST_STMT(ASTClassStmt, ASTClassStmtPtr) {
        Token mName; // 类名称
        ASTListPtr mClassBody;

        AST_DECL();
    };


}

#endif //HYPERMIND_AST_H
